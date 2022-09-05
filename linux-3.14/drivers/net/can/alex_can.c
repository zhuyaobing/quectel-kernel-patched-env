/*
 * alex_can.c - Stoneridge Alexandra CAN
 *
 * Copyright (c) 2021 Stoneridge Electronics
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
//#include <linux/if_arp.h>
//#include <linux/if_ether.h>
#include <linux/can.h>
#include <linux/can/dev.h>
#include <linux/can/skb.h>
#include <linux/slab.h>
#include <linux/of_platform.h>
#include <linux/vmalloc.h>
#include <linux/workqueue.h>
#include <asm/tlbflush.h>
#include <asm/vmapi_calls.h>
//#include <net/rtnetlink.h>

static __initconst const char banner[] =
	KERN_INFO "alex_can: Alexandra CAN\n";

// control_block and raw_can_data structs must match what with CSP
struct control_block {
	uint32_t size;
	volatile uint32_t wr;
	uint32_t rd;
	uint32_t payload_size;
	uint8_t en_flags;
	uint32_t magic;
	uint8_t rsvd[3];
};

#define SHM_MAGIC 0xAEAEAEAE

struct raw_can_data {
	uint64_t timestamp;
	uint32_t id;
	uint16_t channel;
	uint8_t direction;
	uint8_t dlc;
	uint8_t data[8];
	uint8_t type;
	uint8_t ectr;
	uint8_t rsvd[2];
};

#define TYPE_NORMAL 0
#define TYPE_BUSOFF 1
#define TYPE_IDENTITYCHANGE 2

struct alex_can_priv {
	struct can_priv can;	/* must be the first member */
	struct net_device *dev;
	struct napi_struct napi;
	uint32_t channel;

	// shared memory
	const char *filename;
	struct vm_struct *area;
	vmapi_off_t size;
	struct control_block *cb;
	struct raw_can_data *messages;
	uint32_t read_pos;

	// Work queue
	struct delayed_work work;
};

static int alex_can_open(struct net_device *dev)
{
	struct alex_can_priv *priv = netdev_priv(dev);
	int handle;
	int err = 0;
	unsigned int flags = VMAPI_O_R | VMAPI_O_W | VMAPI_O_MAP;

	handle = vmapi_file_open(priv->filename, flags, &(priv->size));
	if(handle < 0) {
		netdev_err(dev, "Failed to open shared memory: %d\n", handle);
		err = -EINVAL;
		goto out;
	}

	err = -ENOMEM;
	if (priv->size > 0) {
		/* get some mmap space */
		priv->area = get_vm_area(priv->size, VM_IOREMAP);
		if (!priv->area) {
			netdev_err(dev, "Unable to get virt memory space\n");
			err = -ENOMEM;
			goto out_close;
		}

		/* protect memory against TLB flushes */
		vm_protect((unsigned long)priv->area->addr, priv->size);
		/* request mapping */
		err = vmapi_file_fmap(handle, 0, priv->size, (unsigned long)priv->area->addr, flags);
		if (err < 0) {
			netdev_err(dev, "fmap failed: %d\n", err);
			goto out_vm_area;
		}

		priv->cb = priv->area->addr;

		if(priv->cb->magic != SHM_MAGIC) {
			netdev_err(dev, "Magic number is not correct: 0x%08X\n", priv->cb->magic);
			goto out_vm_area;
		}

		if(priv->cb->payload_size != sizeof(struct raw_can_data)){
			netdev_err(dev, "Expected data size mismatch\n");
			goto out_vm_area;
		}

		priv->read_pos = priv->cb->wr;
		priv->messages = priv->area->addr + sizeof(struct control_block);

		napi_enable(&priv->napi);
		netif_start_queue(dev);

		// No IRQ from shm so poll with a timer
		schedule_delayed_work(&priv->work, 0);

		err = 0;
		goto out_close;
	}

out_vm_area:
	remove_vm_area(priv->area->addr);
out_close:
	vmapi_file_close(handle);
out:
	return err;
}

static int alex_can_close(struct net_device *dev)
{
	struct alex_can_priv *priv = netdev_priv(dev);

	netif_stop_queue(dev);
	napi_disable(&priv->napi);

	remove_vm_area(priv->area->addr);

	cancel_delayed_work_sync(&priv->work);

	return 0;
}

static netdev_tx_t alex_can_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct can_frame *cf = (struct can_frame *)skb->data;
	struct net_device_stats *stats = &dev->stats;

	if (can_dropped_invalid_skb(dev, skb))
		return NETDEV_TX_OK;

	netdev_err(dev, "id: 0x%08X, dlc: %d\n", cf->can_id, cf->can_dlc);

	stats->tx_packets++;
	stats->tx_bytes += cf->can_dlc;

	if (skb->pkt_type == PACKET_LOOPBACK) {
		/*
		* only count the packets here, because the
		* CAN core already did the echo for us
		*/
		stats->rx_packets++;
		stats->rx_bytes += cf->can_dlc;
	}
	netdev_err(dev, "Sending isn't supported, frame dropped!\n");
	consume_skb(skb);
	return NETDEV_TX_OK;
}

#define CH_RD BIT(1)
#define CH_WR BIT(0)
#define CH_MASK (CH_RD | CH_WR)
#define SET_CH_BITS(ptr, ch, val) set_mask_bits(ptr, CH_MASK << (ch * 2), val << (ch * 2))
static int alex_can_do_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct alex_can_priv *priv = netdev_priv(dev);

	switch (cmd) {
	case SIOCDEVPRIVATE:

		if (priv->cb)
			SET_CH_BITS(&priv->cb->en_flags, priv->channel, rq->ifr_flags);
		else
			return -EIO;

		return 0;

	default:
		return -EOPNOTSUPP;
	}
}

int alex_can_next_type(struct net_device *dev)
{
	struct alex_can_priv *priv = netdev_priv(dev);
	uint32_t read_pos = priv->read_pos;
	uint32_t idx;

	idx = read_pos / sizeof(struct raw_can_data);
	return priv->messages[idx].type;
}

int alex_can_next_channel(struct net_device *dev)
{
	struct alex_can_priv *priv = netdev_priv(dev);
	uint32_t read_pos = priv->read_pos;
	uint32_t idx;

	idx = read_pos / sizeof(struct raw_can_data);
	return priv->messages[idx].channel;
}

void alex_can_read(struct net_device *dev, struct can_frame *cf)
{
	struct alex_can_priv *priv = netdev_priv(dev);
	uint32_t idx;
	uint8_t i;

	if (cf) {
		idx = priv->read_pos / sizeof(struct raw_can_data);

		cf->can_id = priv->messages[idx].id | CAN_EFF_FLAG;
		cf->can_dlc = priv->messages[idx].dlc;
		for (i = 0 ; i < priv->messages[idx].dlc ; ++i) {
			cf->data[i] = priv->messages[idx].data[i];
		}
	}

	priv->read_pos += sizeof(struct raw_can_data);
	if ((priv->read_pos + sizeof(struct raw_can_data)) >= priv->size)
		priv->read_pos = 0;
}

int alex_can_more_messages(struct net_device *dev)
{
	struct alex_can_priv *priv = netdev_priv(dev);
	return priv->cb->wr != priv->read_pos;
}

int alex_can_poll(struct napi_struct *napi, int budget)
{
	struct net_device *dev = napi->dev;
	struct alex_can_priv *priv = netdev_priv(dev);
	int type;
	struct can_frame *cf;
	struct sk_buff *skb;
	int received = 0;

	while (budget > 0) {
		if (!alex_can_more_messages(dev)) {
			napi_complete(napi);
			break;
		}

		if (alex_can_next_channel(dev) != priv->channel) {
			alex_can_read(dev, NULL);
			continue;
		}

		type = alex_can_next_type(dev);
		if (type == TYPE_NORMAL) {
			skb = alloc_can_skb(dev, &cf);
			if (unlikely(!skb)) {
				dev->stats.rx_dropped++;
				return received;
			}

			alex_can_read(dev, cf);
			if (netif_receive_skb(skb) == NET_RX_DROP) {
				netdev_err(dev, "Package dropped, id: 0x%X\n", cf->can_id);
			} else {
				dev->stats.rx_packets++;
				dev->stats.rx_bytes += cf->can_dlc;
			}
		} else if (type == TYPE_BUSOFF) {
			skb = alloc_can_err_skb(dev, &cf);
			if (unlikely(!skb))
				return 0;

			cf->can_id |= CAN_ERR_BUSOFF;
			cf->can_id |= CAN_ERR_CRTL;
			cf->data[1] = CAN_ERR_CRTL_RX_PASSIVE;

			netif_carrier_off(dev);
			priv->can.can_stats.bus_off++;
			dev->stats.rx_errors++;

			netif_receive_skb(skb);

			// Read and discard message
			alex_can_read(dev, NULL);
			break;
		} else {
			// Read and discard message
			alex_can_read(dev, NULL);
			netdev_err(dev, "other message\n");
		}

		received++;
		budget--;
	}
	return received;
}

void alex_can_handler(struct work_struct *work)
{
	struct alex_can_priv *priv;
	priv = container_of(work, struct alex_can_priv, work.work);

	if (alex_can_more_messages(priv->dev))
		napi_schedule(&priv->napi);

	schedule_delayed_work(&priv->work, msecs_to_jiffies(50));
}

static const struct net_device_ops alex_can_netdev_ops = {
	.ndo_open       = alex_can_open,
	.ndo_stop       = alex_can_close,
	.ndo_start_xmit = alex_can_start_xmit,
	.ndo_do_ioctl   = alex_can_do_ioctl,
};

static void alex_can_setup(struct net_device *dev)
{
	dev->netdev_ops = &alex_can_netdev_ops;
}

static int alex_can_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct net_device *dev;
	struct alex_can_priv *priv;
	int err;

	//printk(banner);

	dev = alloc_candev(sizeof(struct alex_can_priv), 0);
	if (dev < 0)
		goto exit_error;
	platform_set_drvdata(pdev, dev);

	alex_can_setup(dev);

	priv = netdev_priv(dev);
	priv->dev = dev;

	err = of_property_read_string(np, "filename", &priv->filename);
	if (err)
		goto exit_free_candev;

	err = of_property_read_u32(np, "channel", &priv->channel);
	if (err)
		goto exit_free_candev;

	netif_napi_add(dev, &priv->napi, alex_can_poll, NAPI_POLL_WEIGHT);

	err = register_candev(dev);
	if (err)
		goto exit_free_candev;

	// No interrupt so drive receiving messages with work queue
	INIT_DELAYED_WORK(&priv->work, alex_can_handler);

	netdev_err(dev, "alex_can: Alexandra CAN channel %d\n", priv->channel);

	return 0;

exit_free_candev:
	free_candev(dev);
exit_error:
	netdev_err(dev,
		"socket CAN driver initialization failed with error: %d\n",
		err);

	return err;
}

static int alex_can_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct alex_can_priv *priv = netdev_priv(dev);

	if (!IS_ERR_OR_NULL(dev))
		unregister_candev(dev);

	if (!IS_ERR_OR_NULL(&priv->napi))
		netif_napi_del(&priv->napi);

	free_candev(dev);

	return 0;
}

static struct of_device_id alex_can_match[] = {
	{ .compatible = "sre,alex-can" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, alex_can_match);

static struct platform_driver alex_can_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
		.owner = THIS_MODULE,
		.of_match_table = alex_can_match,
	},
	.probe = alex_can_probe,
	.remove = alex_can_remove,
};
module_platform_driver(alex_can_driver);

MODULE_DESCRIPTION("Stoneridge Alexandra CAN interface");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Erik Ahlén <erik.ahlen@stoneridge.com>");
