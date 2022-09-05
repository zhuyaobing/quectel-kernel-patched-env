/*
 *  P4VM file provider based virtual network driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 *          Christian Groenke <cgr@sysgo.com>
 * 
 *  Copyright (C) 2005-2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/circ_buf.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/kernthreads.h>
#include <asm/vmapi_calls.h>
#include <asm/locking.h>

/* debug handling */
#define DBG(x...)

#define P4VM_DEV_NAME	"vmfile_net"	/* device name */

#define PACKET_SZ			1536

#define SBUF_MODE_DEACTIVATED	-1
#define SBUF_MODE_INVALID_FD	-2

/* boot stuff */
struct fpeth_boot_info {
#define MAX_FILE_NAME 80
	char name[MAX_FILE_NAME];
	int minor;
};

#define MAX_BOOT_DEVICES 16
static struct fpeth_boot_info fpeth_boot_cmdline[MAX_BOOT_DEVICES] __initdata;

/* actual # of configured boot time devices */
static unsigned int fpeth_boot_num __initdata = 0;

static const char *fpeth_version = "$Id$";


struct fpeth_linkinfo
{
	/* low level thread related */
	spinlock_t lock;
	unsigned int irq;

	/* states part */
	volatile int state_active;
	struct uk_uevent irq_sleep;
	int error;

	/* buffer */
	int inbuf;
	unsigned char *buf;
};


/*
 * per interface private data
 */
struct fpeth_private
{
	int use_counter;

	void *orig_ibuf, *orig_obuf;	/* pointers to preallocated buffers */

	struct sk_buff *rx_skb;
	struct sk_buff *tx_skb;

	/* device related stuff*/
	struct net_device *dev;

	/* mac address handling */
	unsigned char mac[ETH_ALEN];
	int mac_ready;

	/* file related */
	const char *filename;
	int handle;
	int shm;

	/* thread interface */
	struct fpeth_linkinfo rx;
	struct fpeth_linkinfo tx;
};

static struct platform_driver p4vm_driver;

#define P4VM_TYPE_FP
#define P4VM_SHOW_ADDITIONAL_INFO "# <id> <filename> <MAC> <use_counter>"
#include "p4vm_gen_drv_interface.c"

/*
 * 7 prov:/devname 12:34:56:78:9a:cd 0
 * ^      ^                ^         ^
 * |      |                |         +-- Use counter
 * |      |                +------------ MAC address
 * |      +----------------------------- Name of file provider
 * +------------------------------------ eth<x> minor number
 *
 */
static char *p4vm_proc_additional_info(struct p4vm_list *list, char *p)
{
	struct net_device *dev = platform_get_drvdata(list->pdev);
	if (dev) {
		struct fpeth_private* priv = netdev_priv(dev);

		p += sprintf(p, "%d %s %02x:%02x:%02x:%02x:%02x:%02x %d\n",
			list->minor, list->parm1,
			priv->dev->dev_addr[0], priv->dev->dev_addr[1],
			priv->dev->dev_addr[2], priv->dev->dev_addr[3],
			priv->dev->dev_addr[4], priv->dev->dev_addr[5],
			priv->use_counter);
	}
	else {
		/* device not yet registered (probe not called) */
		p += sprintf(p, "%d %s not initialized\n", list->minor, list->parm1);
	}

	return p;
}


/* --------------------------------------------------------------------- */

/*
 * IRQ and Callback related
 */

/*
 * void fpeth_rx_thread(unsigned int irq, void *dev)
 *
 * is irq dispatcher thread and runs in P4 thread context
 *
 * NOTE: do not call any linux functions, as
 *       this code is not executed in linux context!
 */
static void fpeth_rx_thread(unsigned int irq, void *dev)
{
	struct fpeth_private *priv = dev;

	while (1)
	{
		while (!priv->rx.state_active)
		{
			/* sleep until woken */
			sleep_in_irq_thread(&priv->rx.irq_sleep);
		}

		if (priv->handle >= 0)
		{
			DBG("read h:%d, b:%x:\n", priv->handle, priv->rx.buf);

			/* receive msg from thread */
			if(priv->shm >= 0)
				priv->rx.inbuf = vmapi_io_sbuf_read(priv->shm, priv->rx.buf, PACKET_SZ, 0);
			else
				priv->rx.inbuf = vmapi_file_read(priv->handle, priv->rx.buf, PACKET_SZ);

			DBG("got %d\n", priv->rx.inbuf);
			if (priv->rx.inbuf < 0)
			{
				priv->rx.error = priv->rx.inbuf;
				priv->rx.inbuf = 0;
			}
			else
			{
				priv->rx.error = 0;	/* indicates: NO ERROR */
			}
		}
		else
		{
			/* no handle yet, sleep */
			vmapi_delay(1);
			priv->rx.error = 0;
			priv->rx.inbuf = 0;
		}

		/* we "got" an interrupt, now call the handler */
		do_IRQ(irq);
	}
}

/* --------------------------------------------------------------------- */

/*
 * irqreturn_t fpeth_rx_irq(int irq, void *dev, struct pt_regs *regs)
 *
 * called by do_IRQ when buffer received
 */
static irqreturn_t fpeth_rx_irq(int irq, void *dev)
{
	struct fpeth_private *priv = dev;
	struct sk_buff *new_skb;
	unsigned long flags;

	spin_lock_irqsave(&priv->rx.lock, flags);

	/* ignore packets until MAC address is set */
	if (!priv->mac_ready)
	{
		printk(KERN_WARNING "%s: discarding packet, MAC not ready\n",
			   priv->dev->name);
		goto out;
	}

	/* allocate a packet for for next receive cycle */
	new_skb = dev_alloc_skb(PACKET_SZ+2);
	if (new_skb == NULL)
	{
		printk(KERN_WARNING "%s: low on memory, discarding packet!\n",
			   priv->dev->name);
		priv->dev->stats.rx_dropped++;
		goto out;	/* low on memory, discard packet */
	}

	skb_reserve(new_skb, 2);	/* align IP on 16 byte boundary */

	/* send the skbuff */
	skb_put(priv->rx_skb, priv->rx.inbuf);
	priv->rx_skb->dev = priv->dev;
	priv->rx_skb->ip_summed = CHECKSUM_NONE; /* not checked yet */
	priv->rx_skb->protocol = eth_type_trans(priv->rx_skb, priv->dev);
	/*
	 * certain PikeOS drivers like the ne2k_fp use the HW level
	 * multicast to implement virtual ethernet devices. In this case,
	 * our virtual ethernet address is a multicast address, which will
	 * make eth_type_trans() to mark the packet as PACKET_MULTICAST.
	 * However, within the Linux stack, we actually want the packet
	 * to be treated as PACKET_HOST.
	 *
	 * Perhaps it would be cleaner to have ne2k_fp and friends to
	 * hide the multicast tricks alltogether from us (Linux), but
	 * that would mean a much more complicated configuration on the
	 * FP driver's part.
	 */
	if (priv->rx_skb->pkt_type == PACKET_MULTICAST)
		priv->rx_skb->pkt_type = PACKET_HOST;
	netif_rx(priv->rx_skb);

	priv->dev->stats.rx_packets++;
	priv->dev->stats.rx_bytes += priv->rx.inbuf;

	/* prepare buffers for next cycle */
	priv->rx_skb = new_skb;
	priv->rx.buf = priv->rx_skb->data;

out:
	/* reactivate the thread */
	priv->rx.inbuf = 0;

	spin_unlock_irqrestore(&priv->rx.lock, flags);

	return IRQ_HANDLED;
}


/* --------------------------------------------------------------------- */

/*
 * TX state machine
 */

/*
 * void fpeth_tx_thread(unsigned int irq, void *dev)
 *
 * is irq dispatcher thread and runs in P4 thread context
 *
 * NOTE: do not call any linux functions, as
 *       this code is not executed in linux context!
 */
static void fpeth_tx_thread(unsigned int irq, void *dev)
{
	struct fpeth_private *priv = dev;

	while (1)
	{
		while (!priv->tx.state_active)
		{
			/* sleep until woken */
			sleep_in_irq_thread(&priv->tx.irq_sleep);
		}

		if (priv->handle >= 0 && priv->tx.inbuf)
		{
			/* send msg if something to send */
			if(priv->shm >= 0)
				priv->tx.error = vmapi_io_sbuf_write(priv->shm, priv->tx.buf, priv->tx.inbuf, 0);
			else
				priv->tx.error = vmapi_file_write(priv->handle, priv->tx.buf, priv->tx.inbuf);

			if (priv->tx.error >= 0)
			{
				/* sucessfully send */
				priv->tx.error = 0;
			}
		}
		else
		{
			/* no handle yet, error ... */
			vmapi_delay(1);
			priv->tx.error = 1;
		}

		/* transfer execution to linux IRQ handler */
		do_IRQ(irq);
	}
}

/*
 * irqreturn_t fpeth_tx_irq(int irq, void *dev, struct pt_regs *regs)
 *
 * called by do_IRQ when buffer transmitted
 */
static irqreturn_t fpeth_tx_irq(int irq, void *dev)
{
	struct fpeth_private *priv = dev;
	unsigned long flags;

	spin_lock_irqsave(&priv->tx.lock, flags);

	/* disarm thread */
	priv->tx.state_active = 0;

	priv->tx.buf = 0;
	priv->tx.inbuf = 0;

	if (priv->tx_skb)
	{
		priv->dev->stats.tx_packets++;
		priv->dev->stats.tx_bytes += priv->tx_skb->len;
		/* do not free skb here ... do it on next transmit */
	}

	/* wake net queue */
	netif_wake_queue(priv->dev);

	spin_unlock_irqrestore(&priv->tx.lock, flags);

	return IRQ_HANDLED;
}

/*
 * Transmit a single packet (place it in the TX state machine)
 */
static int fpeth_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct fpeth_private *priv = netdev_priv(dev);
	unsigned long flags;
	struct sk_buff *freeskb;
	int err;

	spin_lock_irqsave(&priv->tx.lock, flags);

	netif_stop_queue(dev);

	if (priv->tx.inbuf)
	{
		/* output "queue" full */
		DBG("output queue full!\n");
		spin_unlock_irqrestore(&priv->tx.lock, flags);
		return NETDEV_TX_BUSY;
	}

	/* enqueue skb and wakeup TX thread */
	freeskb = priv->tx_skb;
	priv->tx_skb = skb;
	priv->tx.inbuf = skb->len;
	priv->tx.buf = skb->data;

	if(priv->shm >= 0) {
		err = vmapi_io_sbuf_write(priv->shm, priv->tx.buf, priv->tx.inbuf, 1);
		if (err >= 0)
		{
			/* mimick 'tx_thread' and 'tx_irq' here */
			
			/* clear counter */
			priv->tx.buf = 0;
			priv->tx.inbuf = 0;
			
			priv->dev->stats.tx_packets++;
			priv->dev->stats.tx_bytes += err;
			
			netif_wake_queue(priv->dev);
			
			/* sucessfully send */
			priv->tx.error = 0;
		} else if (err == -EWOULDBLOCK) {
			priv->tx.state_active = 1;

			wake_irq_thread(&priv->tx.irq_sleep);
		} else {
			/* error - currently no one cares however */
			priv->tx.error = 1;
			netif_wake_queue(priv->dev);
		}
	}
	else {
		priv->tx.state_active = 1;

		wake_irq_thread(&priv->tx.irq_sleep);
	}

	spin_unlock_irqrestore(&priv->tx.lock, flags);

	/* free old skb */
	if (freeskb)
		dev_kfree_skb(freeskb);

	return NETDEV_TX_OK;
}


/* --------------------------------------------------------------------- */

/*
 * IRQ allocator
 */

static int fpeth_get_irqs(struct fpeth_private *priv)
{
	int rx_irq;
	int tx_irq;
	int err = -ENODEV;

	rx_irq = get_free_async_irq();
	tx_irq = get_free_async_irq();

	if (!rx_irq || !tx_irq) {
		printk(KERN_ERR "%s: unable to get free irqs\n", priv->dev->name);
		goto release_irqs;
	}

	/* allocate RX IRQ */
	if (request_irq(rx_irq, fpeth_rx_irq, 0, priv->dev->name, priv)) {
		printk(KERN_ERR "%s: unable to request read-IRQ %d\n", priv->dev->name,
			   rx_irq);
		goto release_irqs;
	}
	priv->rx.irq = rx_irq;

	/* allocate TX IRQ */
	if (request_irq(tx_irq, fpeth_tx_irq, 0, priv->dev->name, priv)) {
		printk(KERN_ERR "%s: unable to request write-IRQ %d\n", priv->dev->name,
			   tx_irq);
		goto free_rx;
	}
	priv->tx.irq = tx_irq;

	/* restart IRQ threads with proper callback */
	err = enable_irq_thread(priv->rx.irq, "vmfile_net_rx", fpeth_rx_thread, priv);
	if (err) {
		printk(KERN_ERR "%s: could not start async IRQ RX thread\n", priv->dev->name);
		err = -ENOMEM;
		goto free_tx;
	}
	
	err = enable_irq_thread(priv->tx.irq, "vmfile_net_tx", fpeth_tx_thread, priv);
	if (err) {
		disable_irq_thread(priv->rx.irq);
		
		printk(KERN_ERR "%s: could not start async IRQ TX thread\n", priv->dev->name);
		err = -ENOMEM;
		goto free_tx;
	}

	return 0;

free_tx:
	free_irq(priv->tx.irq, priv);

free_rx:
	free_irq(priv->rx.irq, priv);

release_irqs:
	if (rx_irq)
		release_async_irq(rx_irq);
	if (tx_irq)
		release_async_irq(tx_irq);

	return err;
}

static void fpeth_put_irqs(struct fpeth_private *priv)
{
	disable_irq_thread(priv->rx.irq);
	disable_irq_thread(priv->tx.irq);
	free_irq(priv->rx.irq, priv);
	free_irq(priv->tx.irq, priv);

	release_async_irq(priv->rx.irq);
	release_async_irq(priv->tx.irq);
}

/* --------------------------------------------------------------------- */

/*
 * open / close handling
 */

static int fpeth_open(struct net_device *dev)
{
	struct fpeth_private *priv;
	int err = 0;
	const char *mac_addr_src = "default";
	unsigned int size;
	vmapi_off_t size_o;
	unsigned int *vaddr = NULL;

	if (dev == NULL)
		return -ENODEV;

	priv = netdev_priv(dev);

	/* check use counters: */
	if (priv->use_counter)
		return -EBUSY;

	priv->rx_skb = 0;
	priv->tx.buf = 0;
	priv->tx_skb = 0;

	/* these variables may be undefined when the TX/RX thread was killed
	 * by fpeth_close -> reset to default
	 */
	priv->rx.state_active = 0;
	priv->tx.state_active = 0;
	uk_uevent_init(&priv->tx.irq_sleep);
	uk_uevent_init(&priv->rx.irq_sleep);

	/* update counters and types */
	priv->use_counter++;

	/* open the file provider */
	priv->handle = vmapi_file_open4(priv->filename, VMAPI_O_R | VMAPI_O_W | VMAPI_O_MAP, &size_o, NULL);
	if (priv->handle < 0)
	{
		/* retry, maybe we have no mapping rights */
		priv->handle = vmapi_file_open4(priv->filename, VMAPI_O_R | VMAPI_O_W, &size_o, NULL);
		if (priv->handle < 0) {
			err = priv->handle;
			printk(KERN_WARNING "%s: error %d opening PikeOS file provider %s\n", 
				   dev->name, err, priv->filename);
		}
		else {
			priv->shm = SBUF_MODE_DEACTIVATED;
		}
	} else {
		priv->shm = SBUF_MODE_INVALID_FD;
	}
	size = size_o;

	/* read MAC address if not already set */
	if (!priv->mac_ready && !err)
	{
		err = vmapi_get_eth_mac(priv->handle, (unsigned char*)&priv->mac);
		if (err) {
			printk(KERN_WARNING "%s: error %d reading MAC address\n", 
				   dev->name, err);
		}
		else
		{
			priv->mac_ready = 1;
			/* autodetect mac address ... */
			mac_addr_src = "autodetected";
			memcpy(dev->dev_addr, priv->mac, ETH_ALEN);
		}
	}

	/* prepare driver for sbuf usage */
	if (!err && priv->shm != SBUF_MODE_DEACTIVATED)
	{
		err = vmapi_io_sbuf_init(priv->handle, &size);
		if(!err)
		{
			/* get contigous virtual memory area 
			 * we need a bit more memory to account for the aligment correction
			 * FIXME: test if maybe one page more is enough
			 */
			vaddr = vmalloc(size+2*PAGE_SIZE);
			if (vaddr == NULL)
			{
				err = -ENOMEM;
				printk(KERN_ERR "%s: coudld not allocate enough vmem - size:%u err: %i\n", dev->name, size, err);
			}
			else
			{
				/* make shure we have a page aligned pointer */
				vaddr = (unsigned int *)(((unsigned long)vaddr + PAGE_SIZE -1) & PAGE_MASK);
				/* map the SHM-Buf into our memory space */
				err = vmapi_io_sbuf_map(priv->handle, (unsigned long)vaddr);
				if(err >= 0)
					priv->shm = err;
				else
					printk(KERN_ERR "%s: coudld not map shared memory - err: %i\n", dev->name, err);
			}
		}
		else
		{
			/* If we could not init libsbuf we report it as INFO and try to setup normal FP Mode */
			printk(KERN_INFO "%s: coudld not init libsbuf - err: %i\n", dev->name, err);
			
			/* clear error to allow driver the start in 'normal' mode */
			err = 0;
			/* set shm file handle to an invalid value */
			priv->shm = -1;
		}
	}

	if (!err)
		err = fpeth_get_irqs(priv);

	if (!err)
	{
		/* prepare receive "pseudo dma" buffer */
		priv->rx_skb = dev_alloc_skb(PACKET_SZ+2);
		if (priv->rx_skb == NULL)
			err = -ENOMEM;
		else
		{
			skb_reserve(priv->rx_skb, 2);	/* align IP on 16 byte boundary */

			priv->rx.buf = priv->rx_skb->data;
		}
	}

	if (err)
	{
		if (priv->handle >= 0)
		{
			if(vaddr != NULL)
				vfree(vaddr);
			priv->shm = -1;

			vmapi_file_close(priv->handle);
			priv->handle = -1;
		}
		if (priv->rx_skb) dev_kfree_skb(priv->rx_skb);
		priv->use_counter--;
		return err;
	}

	/* set network specific stuff */
	dev->flags &= ~IFF_MULTICAST;

	/* enable reception thread */
	priv->rx.state_active = 1;
	priv->tx.state_active = 0;
	wake_irq_thread(&priv->rx.irq_sleep);

	/* 
	 * inform the user which mode we are using
	 * NOTE:
	 *    The user will only see this message if libshm could or couldn't be initialized
	 *    If there was another error, hence the mapping failed, the driver will return before
	 *    it reaches this code
	 */
	if(priv->shm >= 0)
		printk(KERN_INFO "%s: Starting in shared buffer mode\n", dev->name);
	else
		printk(KERN_INFO "%s: Starting in normal FP mode\n", dev->name);

	printk(KERN_INFO "%s: using %s mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
	       dev->name, mac_addr_src,
	       dev->dev_addr[0], dev->dev_addr[1],
	       dev->dev_addr[2], dev->dev_addr[3],
	       dev->dev_addr[4], dev->dev_addr[5]);

	netif_start_queue(dev);

	return 0;
}

static int fpeth_close(struct net_device *dev)
{
	struct fpeth_private *priv;

	if (dev == NULL)
		return -ENODEV;

	netif_stop_queue(dev);

	priv = netdev_priv(dev);

	priv->use_counter--;

	priv->rx.state_active = 0;
	priv->tx.state_active = 0;

	/* disable both IRQ threads */
	fpeth_put_irqs(priv);

	/* make shure the sbuf handle can be reused */
	vmapi_io_sbuf_close(priv->shm);
	priv->shm = -1;

	vmapi_file_close(priv->handle);
	priv->handle = -1;

	/* free old skbuffs */
	if (priv->rx_skb)
		dev_kfree_skb(priv->rx_skb);
	if (priv->tx_skb)
		dev_kfree_skb(priv->tx_skb);

	priv->rx_skb = 0;
	priv->tx_skb = 0;

	return 0;
}

static int fpeth_set_mac_address(struct net_device *dev, void *addr)
{
	printk(KERN_INFO P4VM_DEV_NAME ": Changing the MAC address in Linux is not supported.\n");
	printk(KERN_INFO "            Please use the network driver settings in the vmit.xml.\n");
	return -EOPNOTSUPP;
}

static int fpeth_validate_addr(struct net_device *dev)
{
	/* always valid (set on open) */
	return 0;
}

/* --------------------------------------------------------------------- */

/*
 * ETHtool interface
 */

static void fpeth_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strncpy(info->driver, dev->name, sizeof(info->driver)-1);
	strncpy(info->version, fpeth_version, sizeof(info->version)-1);
}

static int fpeth_get_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
	int link;
	int speed;
	struct fpeth_private *priv = netdev_priv(dev);

	if (vmapi_get_eth_link_stat(priv->handle, &link, &speed))
		return -1;

	/* the faked settings */
	ecmd->port        = PORT_MII;
	ecmd->transceiver = XCVR_INTERNAL;
	ecmd->duplex      = DUPLEX_FULL;

	/* the only one we can get */
	ecmd->speed = speed;
	return 0;
}

static u32 fpeth_get_link(struct net_device *dev)
{
	int link;
	int speed;
	struct fpeth_private *priv = netdev_priv(dev);

	/* try to get the infos from our low-level driver */
	if (vmapi_get_eth_link_stat(priv->handle, &link, &speed))
		return 0; /* no link on error ! */
	
	if (link)
		return 1;

	return 0;
}

static struct ethtool_ops ethtool_ops = {
	.get_drvinfo  = fpeth_get_drvinfo,
	.get_settings = fpeth_get_settings,
	.get_link     = fpeth_get_link,
};

static const struct net_device_ops fpeth_netdev_ops = {
	.ndo_open		= fpeth_open,
	.ndo_stop		= fpeth_close,
	.ndo_start_xmit		= fpeth_tx,
	.ndo_validate_addr	= fpeth_validate_addr,
	.ndo_set_mac_address	= fpeth_set_mac_address,
};

/* --------------------------------------------------------------------- */


/* --------------------------------------------------------------------- */

/*
 * Initialize and register driver
 */
static int fpeth_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct fpeth_private *priv;
	struct resource *res;
	int err;

	res = platform_get_resource(pdev, IORESOURCE_BUS, 0);
	if (!res) {
		dev_warn(&pdev->dev, "Resource not set\n");
		return -EINVAL;
	}

	/* allocate an ethernet device and private data */
	dev = alloc_etherdev(sizeof(struct fpeth_private));
	if (dev == NULL)
		return -ENOMEM;

	priv = netdev_priv(dev);

	/* init part I: minimum setup before adding to our devlist */
	priv->dev = dev;
	priv->use_counter = 0;
	priv->filename = res->name;
	priv->handle = -1;
	spin_lock_init(&priv->tx.lock);
	spin_lock_init(&priv->rx.lock);

	sprintf(dev->name, "eth%d", pdev->id);

	/* init part II: callbacks etc */
	ether_setup(dev);

	dev->netdev_ops  = &fpeth_netdev_ops;
	dev->ethtool_ops = &ethtool_ops;
#if 0
	dev->flags |= IFF_NOARP;
	dev->hard_header_cache = NULL;
#endif

	err = register_netdev(dev);
	if (err) {
		printk(KERN_WARNING P4VM_DEV_NAME ": error %d registering network device %s\n",
		       err, dev->name);
		free_netdev(dev);
	}

	platform_set_drvdata(pdev, dev);

	return err;
}

static int fpeth_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);

	unregister_netdev(dev);
	free_netdev(dev);

	return 0;
}

static struct platform_driver p4vm_driver = {
	.driver	= {
		.name	= P4VM_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe   = fpeth_probe,
	.remove = fpeth_remove,
};

static int __init fpeth_init(void)
{
	struct fpeth_boot_info *bi;
	unsigned int dev;
	int err;

	err = platform_driver_register(&p4vm_driver);
	if (err < 0)
		return err;

	err = p4vm_driver_init();
	if (err)
		return err;

	for (dev = 0; dev < fpeth_boot_num; dev++) {
		bi = &fpeth_boot_cmdline[dev];

		err = p4vm_add_interface(bi->minor, bi->name, NULL);
		if (err == 0)
		{
			printk(KERN_INFO P4VM_DEV_NAME ": added boot inferface eth%d using %s\n",
			       bi->minor, bi->name);
		}
		else
		{
			printk(KERN_ERR P4VM_DEV_NAME ": failed to add boot inferface eth%d using %s; error %d\n",
			       bi->minor, bi->name, err);
			return err;
		}
	}

	return 0;
}


/*
 * Shutdown driver
 */
static void __exit fpeth_exit(void)
{
	p4vm_driver_exit();

	/* will call remove function for all connected devices */
	platform_driver_unregister(&p4vm_driver);
}

#ifndef MODULE

/*
 * parse vmfile_net=... kernel command-line
 *   vmfile_net=<minor>,<filename>
 *   vmfile_net=0,eth0:/1
 */
static int __init fpeth_setup(char *options)
{
	struct fpeth_boot_info *bi;
	int size;
	char *n;

	DBG("fpeth_setup: got %s\n", options);

	if (fpeth_boot_num >= MAX_BOOT_DEVICES)
		return 0;

	bi = &fpeth_boot_cmdline[fpeth_boot_num];

	/* get first substring: minor(int) */
	bi->minor = simple_strtol(options, &n, 0);

	if (n == options || *n != ',' || *n == '\0')
		return 0;	/* bad */

	/* hop over the comma */
	options = n+1;

	/* copy second substring: filename */
	n = bi->name;
	size = 0;
	while (options && *options && *options != ',' && size < MAX_FILE_NAME-1)
	{
		*n++ = *options++;
		size++;
	}
	*n = 0;

	if (!size)
		return 0;	/* bad */

	/* mark bootargs as valid */
	fpeth_boot_num++;

	return 0;
}

__setup("vmfile_net=", fpeth_setup);
#endif

module_init(fpeth_init);
module_exit(fpeth_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS File Provider (client) virtual network driver");
MODULE_LICENSE("GPL");
