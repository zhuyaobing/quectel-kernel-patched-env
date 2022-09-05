/*
 *  P4VM port "virtual network" driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2004-2011 SYSGO AG
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
#include <linux/console.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/kernthreads.h>
#include <asm/vmapi_calls.h>
#include <asm/locking.h>
#include <asm/host_tasks.h>
#include "vmport_core.h"

#define HANDLE_NDS_MSG	/* autodetect mac address */
#include "nds.h"

#define RETRYMAC 10	/* MAC address retry count */

#define DBG(x...)

#define HELMUT_DEV_NAME			"vmport_net"	/* device name */

#define PACKET_SZ			1536


/* boot stuff */
struct helmut_boot_info {
#define MAX_PORT_NAME 32
	char iname[MAX_PORT_NAME];
	char oname[MAX_PORT_NAME];
	unsigned char mac[ETH_ALEN];
	int mac_provided;
	int minor;
};

#define MAX_BOOT_DEVICES 16
static struct helmut_boot_info helmut_boot_cmdline[MAX_BOOT_DEVICES];

/* actual # of configured boot time devices */
static unsigned int helmut_boot_num = 0;

static struct platform_device *helmut_platform_device;

/*
 * per interface private data
 */
struct helmut_private
{
	struct list_head list;		/* link in list of devices */
	unsigned int minor;	/* interface eth%d */
	int use_counter;

	/* connected ports */
	struct vmport_core_portinfo *iport;
	struct vmport_core_portinfo *oport;

	struct sk_buff *rx_skb;
	struct sk_buff *tx_skb;

	wait_queue_head_t await_mac;

	/* device related stuff*/
	struct net_device *dev;

	/* mac address handling */
	int mac_ready;
	int silence;

	int tx_irq;
	struct uk_uevent tx_irq_sleep;
};

/*
 * global variables
 */
static struct list_head helmut_devlist;		/* link in all/new_channels list */
static spinlock_t helmut_lock = __SPIN_LOCK_UNLOCKED(helmut_lock);	/* protects driver_info ... */

/* --------------------------------------------------------------------- */

/*
 * construct a special MAC address:
 *
 * 02:53:59:tt:tm:mm
 *
 * 02                  is for local administrated MAC addresses
 *    53 59            is "SY"
 *          tt t       is the kernels task number
 *              m mm   is the minor device
 */
static void helmut_mk_special_mac(unsigned char *mac, unsigned int minor)
{
	unsigned int task = uk_task_id();

	mac[0] = 0x02;
	mac[1] = 0x53;
	mac[2] = 0x59;
	mac[3] = (task >> 4) & 0xff;
	mac[4] = ((task << 4) & 0xf0) | ((minor >> 8) & 0x0f);
	mac[5] = minor & 0xff;
}


/* --------------------------------------------------------------------- */

/*
 * Async thread and interrupt routines
 */

/*
 * IRQ callback (called only when a message was read)
 */
static void helmut_rx_int(void *data)
{
	struct helmut_private *priv = data;
	struct sk_buff *new_skb;
	int size;

	size = priv->iport->buf_size;
	if (size <= 0 || size > PACKET_SZ)
	{
		printk(KERN_INFO "%s: discarding packet (err = %d)\n", priv->dev->name, size);
		return;
	}

#ifdef HANDLE_NDS_MSG
	/* special handling for "net driver sync messages" */
	if (sizeof(struct nds_msg) == size)
	{
		struct nds_msg *msg = (struct nds_msg *)priv->iport->buf;

		/* ignore errors */
		if (msg->type == NDS_CMD_GET_MAC && !priv->mac_ready &&
		    msg->error == NDS_E_OK)
		{
			priv->mac_ready = 1;
			memcpy(priv->dev->dev_addr, &msg->u.mac, ETH_ALEN);
			/* notify helmut_open() */
			wake_up_interruptible(&priv->await_mac);

			DBG("got mac address %02x:%02x:%02x:%02x:%02x:%02x!\n",
				msg->u.mac[0], msg->u.mac[1], msg->u.mac[2],
				msg->u.mac[3], msg->u.mac[4], msg->u.mac[5]);
		}

		return;
	}
#endif

	/* ignore packets until MAC address is set */
	if (!priv->mac_ready)
	{
		if (priv->silence == 0) {
			printk(KERN_INFO "%s: discarding packet, MAC not ready\n", 
				   priv->dev->name);
		}
		priv->silence = 1;
		return;
	}
	priv->silence = 0;

	/* allocate a packet for for next receive cycle */
	new_skb = dev_alloc_skb(PACKET_SZ+2);
	if (new_skb == NULL)
	{
		printk(KERN_WARNING "%s: low on memory, discarding packet!\n", 
			   priv->dev->name);
		priv->dev->stats.rx_dropped++;
		return;	/* low on memory, discard packet */
	}

	skb_reserve(new_skb, 2);	/* align IP on 16 byte boundary */

	/* send the skbuff */
	skb_put(priv->rx_skb, size);
	priv->rx_skb->dev = priv->dev;
	priv->rx_skb->ip_summed = CHECKSUM_NONE; /* not checked yet */
	priv->rx_skb->protocol = eth_type_trans(priv->rx_skb, priv->dev);
	netif_rx(priv->rx_skb);

	priv->dev->stats.rx_packets++;
	priv->dev->stats.rx_bytes += size;

	/* prepare buffers for next cycle */
	priv->rx_skb = new_skb;
	priv->iport->buf = priv->rx_skb->data;
}


/*
 * void helmut_tx_thread(unsigned int irq, void *data)
 *
 * send qport message in P4 thread context
 *
 */
static void helmut_tx_thread(unsigned int irq, void *data)
{
	struct helmut_private *priv = data;

	while (1)
	{
		sleep_in_irq_thread(&priv->tx_irq_sleep);
		vmapi_qport_send(priv->oport->line, priv->tx_skb->data, priv->tx_skb->len);

		/** notify Linux */
		do_IRQ(priv->tx_irq);
	}
}

/*
 * P4 thread wake up -> Linux with this IRQ
 */
static irqreturn_t helmut_tx_irq(int irq, void *data)
{
	struct helmut_private *priv = data;

	priv->oport->buf = 0;

#ifdef HANDLE_NDS_MSG
	/* special handling for nds messages */
	if (sizeof(struct nds_msg) == priv->tx_skb->len)
	{
		/* do not wake netqueue etc */
		dev_kfree_skb_irq(priv->tx_skb);
		priv->tx_skb = 0;
		return IRQ_HANDLED;
	}
#endif

	if (priv->tx_skb)
	{
		priv->dev->stats.tx_packets++;
		priv->dev->stats.tx_bytes += priv->tx_skb->len;
		dev_kfree_skb_irq(priv->tx_skb);
		priv->tx_skb = 0;
	}

	netif_wake_queue(priv->dev);

	return IRQ_HANDLED;
}

/* --------------------------------------------------------------------- */

/*
 * Transmit a single packet (place it in the TX state machine)
 */
static int helmut_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct helmut_private *priv = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&priv->oport->lock, flags);

	netif_stop_queue(dev);

	if (priv->tx_skb) {
		/* output "queue" full */
		DBG("output queue full!\n");
		spin_unlock_irqrestore(&priv->oport->lock, flags);
		return NETDEV_TX_BUSY;
	}

	/* enqueue skb and wakeup TX thread */
	priv->tx_skb = skb;
	wake_irq_thread(&priv->tx_irq_sleep);

	spin_unlock_irqrestore(&priv->oport->lock, flags);

	return NETDEV_TX_OK;
}

#ifdef HANDLE_NDS_MSG
static void helmut_request_mac_address(struct helmut_private *priv)
{
	struct sk_buff *skb;
	struct nds_msg *msg;
	unsigned long flags;
	unsigned int nb_messages, max_messages;
	int ret;

	/** check if free space available available */
	ret = vmport_core_stat_num_msg(priv->oport, &nb_messages, &max_messages);
	if (ret) {
		printk(KERN_WARNING "%s: get status failed, cannot request mac address!\n",
			   priv->dev->name);
		return;
	}

	skb = dev_alloc_skb(PACKET_SZ+2);
	if (skb == NULL)
	{
		printk(KERN_WARNING "%s: low on memory, cannot request mac address!\n",
			   priv->dev->name);
		return;
	}

	skb_reserve(skb, 2);	/* align IP on 16 byte boundary */
	skb_put(skb, sizeof(struct nds_msg));
	skb->dev = priv->dev;

	/* build request message */
	msg = (struct nds_msg *)skb->data;
	msg->type = NDS_CMD_GET_MAC;
	msg->error = NDS_E_REQ;
	msg->sequence = 0;	/* not used yet */

	spin_lock_irqsave(&priv->oport->lock, flags);

	if (0 == nb_messages)
	{
		printk(KERN_WARNING "%s: could not send request message! (no free space)\n", 
			   priv->dev->name);
		/* queue full, free skb */
		dev_kfree_skb(skb);
		spin_unlock_irqrestore(&priv->oport->lock, flags);
		return;
	}

	/* enqueue skb and wakeup TX thread */
	priv->tx_skb = skb;
	priv->oport->buf = skb->data;
	spin_unlock_irqrestore(&priv->oport->lock, flags);

	/** re-activate the thread */
	wake_irq_thread(&priv->tx_irq_sleep);
}
#endif

/* --------------------------------------------------------------------- */

/*
 * open / close handling
 */

static int helmut_open(struct net_device *dev)
{
	struct helmut_private *priv;
	unsigned long flags;
	int err = 0;
	const char *mac_addr_src = "default";
	struct sk_buff *skb;

	if (dev == NULL)
		return -ENODEV;

	priv = netdev_priv(dev);

	BUG_ON(priv->rx_skb);
	BUG_ON(priv->tx_skb);

	/* prepare receive "pseudo dma" buffer */
	priv->rx_skb = dev_alloc_skb(PACKET_SZ+2);
	priv->tx_skb = 0;

	if (priv->rx_skb == NULL)
		return -ENOMEM;

	skb_reserve(priv->rx_skb, 2);	/* align IP on 16 byte boundary */

	spin_lock_irqsave(&helmut_lock, flags);

	/* check use counters: */
	if (priv->use_counter)
	{
		err = -EBUSY;
		spin_unlock_irqrestore(&helmut_lock, flags);
		goto out_release;
	}

	/* update counters and types */
	priv->use_counter++;

	spin_unlock_irqrestore(&helmut_lock, flags);

	err = vmport_core_init_port(priv->iport, helmut_rx_int, priv,
				    VMPORT_CORE_TYPE_NET, priv->rx_skb->data, 1);
	if (err) {
		printk(KERN_ERR HELMUT_DEV_NAME ": cound not initialize port <%s> - err: %d\n",
		       priv->iport->portname, err);
		goto out_counter;
	}

	/* pass pseudo buf to force external buffer handling) */
	err = vmport_core_init_port(priv->oport, NULL, priv,
				    VMPORT_CORE_TYPE_NET, priv->rx_skb->data, 0);
	if (err) {
		printk(KERN_ERR HELMUT_DEV_NAME ": cound not initialize port <%s> - err: %d\n",
		       priv->oport->portname, err);
		vmport_core_exit_port(priv->iport);
		goto out_counter;
	}

	/* set network specific stuff */
	dev->flags &= ~IFF_MULTICAST;

	vmport_core_poll_start(priv->iport);

	/* activate transmit thread */
	uk_uevent_init(&priv->tx_irq_sleep);
	priv->tx_irq = get_free_async_irq();
	if (!priv->tx_irq ||
	    enable_irq_thread(priv->tx_irq, dev->name,helmut_tx_thread, priv) || 
	    request_irq(priv->tx_irq, helmut_tx_irq, 0, dev->name, priv)) {
		printk(KERN_ERR HELMUT_DEV_NAME ": cound not initialize IRQ %d\n",
		       priv->tx_irq);
		err = -ENODEV;
		spin_lock_irqsave(&helmut_lock, flags);
		goto out_counter;
	}

#ifdef HANDLE_NDS_MSG
	/* endless loop until we got the MAC address */
	if (!priv->mac_ready)
	{
		DBG("requesting mac address ...\n");
		for (err = 0; err < RETRYMAC; err++)
		{
			/* ports are up, ready to autodetect mac address */
			helmut_request_mac_address(priv);

			/* wait for response */
			if (interruptible_sleep_on_timeout(&priv->await_mac, HZ / 10))
				break;

			/* check if mac address is available */
			spin_lock_irqsave(&priv->oport->lock, flags);

			if (priv->mac_ready)
			{
				spin_unlock_irqrestore(&priv->oport->lock, flags);
				break;
			}
			spin_unlock_irqrestore(&priv->oport->lock, flags);
		}
	}

	/* check if mac address is available */
	spin_lock_irqsave(&priv->oport->lock, flags);

	if (!priv->mac_ready)
	{
		/* error retrieving MAC address ... release resources */
		mac_addr_src = "default";
		helmut_mk_special_mac(dev->dev_addr, priv->minor);
	}
	else if (priv->mac_ready == 2)
	{
		/* MAC address provided by "proc" interface */
		mac_addr_src = "provided";
	}
	else
	{
		/* autodetected mac address */
		mac_addr_src = "autodetected";
	}
	priv->mac_ready = 1;
	spin_unlock_irqrestore(&priv->oport->lock, flags);
#endif
	printk(KERN_INFO "%s: using %s mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
	       dev->name, mac_addr_src,
	       dev->dev_addr[0], dev->dev_addr[1],
	       dev->dev_addr[2], dev->dev_addr[3],
	       dev->dev_addr[4], dev->dev_addr[5]);
	netif_start_queue(dev);

	return 0;

out_counter:
	priv->use_counter--;
out_release:
	DBG("device busy\n");
	skb = priv->rx_skb;
	priv->rx_skb = 0;
	dev_kfree_skb(skb);
	return err;
}

static int helmut_close(struct net_device *dev)
{
	struct helmut_private *priv;
	unsigned long flags;

	if (dev == NULL)
		return -ENODEV;

	netif_stop_queue(dev);

	priv = netdev_priv(dev);
	/* disable receive thread */
	vmport_core_poll_stop(priv->iport);

	/* disable transmit thread */
	vmport_core_poll_stop(priv->oport);
	disable_irq_thread(priv->tx_irq);
	release_async_irq(priv->tx_irq);
	free_irq(priv->tx_irq, priv);

	spin_lock_irqsave(&helmut_lock, flags);
	priv->use_counter--;

	vmport_core_exit_port(priv->iport);
	vmport_core_exit_port(priv->oport);

	spin_unlock_irqrestore(&helmut_lock, flags);

	/* free old skbuffs */
	if (priv->rx_skb)
		dev_kfree_skb(priv->rx_skb);
	if (priv->tx_skb)
		dev_kfree_skb(priv->tx_skb);

	priv->rx_skb = 0;
	priv->tx_skb = 0;

	return 0;
}

static int helmut_set_mac_address(struct net_device *dev, void *addr)
{
	struct helmut_private *priv = netdev_priv(dev);
	int ret;

	/* call standard function */
	ret = eth_mac_addr(dev, addr);
	if (ret == 0)
		priv->mac_ready = 2;

	return ret;
}

static int helmut_validate_addr(struct net_device *dev)
{
	/* always valid (set on open) */
	return 0;
}

static const struct net_device_ops helmut_netdev_ops = {
	.ndo_open		= helmut_open,
	.ndo_stop		= helmut_close,
	.ndo_start_xmit		= helmut_tx,
	.ndo_validate_addr	= helmut_validate_addr,
	.ndo_set_mac_address	= helmut_set_mac_address,
};

/* --------------------------------------------------------------------- */

/*
 * add and remove interface stuff below
 */

/*
 * int helmut_add_interface(minor, inport, outport)
 *
 *  adds interface "eth<minor>", connected with ports idev and odev
 */
static int helmut_add_interface(unsigned int minor, const char *idev,
				const char *odev, void *mac, int provided_mac)
{
	struct vmport_core_portinfo *port;
	struct net_device *dev;
	struct helmut_private *priv, *temp;
	unsigned long flags;
	int i;
	int err;

	/* allocate an ethernet device and private data */
	dev = alloc_etherdev(sizeof(struct helmut_private));
	if (dev == NULL)
	{
		err = -ENOMEM;
		goto free;
	}

	/* init part I: minimum setup before adding to our devlist */
	priv = netdev_priv(dev);
	memset(priv, 0, sizeof(*priv));

	priv->dev = dev;
	priv->minor = minor;
	priv->use_counter = 0;

	init_waitqueue_head(&priv->await_mac);

	/* scan list of ports if these are OK */
	for (i = 0; i < vmport_core_count; i++)
	{
		port = &vmport_core_portinfo[i];
		if (strcmp(port->portname, idev) == 0 ||
			strcmp(port->portname, odev) == 0)
		{
			if (port->framesize != PACKET_SZ) {
				printk(KERN_WARNING "%s: size of %s = %d, must be %d\n", 
						dev->name, port->portname, port->framesize, PACKET_SZ);
			}
			else if (port->direction == VMPORT_CORE_DIR_RECV) {
				if (priv->iport) {
					printk(KERN_WARNING "%s: RX port already set, ignoring %s \n", 
							dev->name, port->portname);
				} else {
					priv->iport = port;
				}
			}
			else if (port->direction == VMPORT_CORE_DIR_SEND) {
				if (priv->oport) {
					printk(KERN_WARNING "%s: TX port already set, ignoring %s \n", 
							dev->name, port->portname);
				} else {
					priv->oport = port;
				}
			}
		}
	}

	if (priv->iport == NULL || priv->oport == NULL)
	{
		err = -ENODEV;
		goto free;
	}

	/* ports are valid, now check if <num> is not used yet */
	spin_lock_irqsave(&helmut_lock, flags);
	/* scan the list ...*/
	list_for_each_entry(temp, &helmut_devlist, list) {
		if (temp->minor == minor)
		{
			spin_unlock_irqrestore(&helmut_lock, flags);
			err = -EBUSY;
			goto free;	/* whee, device with same number already in use */
		}
	}

	/* ok, add dev to list and release the lock */
	list_add_tail(&priv->list, &helmut_devlist);
	spin_unlock_irqrestore(&helmut_lock, flags);

	sprintf(dev->name, "eth%d", minor);

	/* init part II: callbacks etc */
	ether_setup(dev);

	dev->netdev_ops  = &helmut_netdev_ops;
#if 0
	dev->flags |= IFF_NOARP;
#endif

	/* set mac address if provided, else let open autotect it */
	if (provided_mac) {
		memcpy(dev->dev_addr, mac, ETH_ALEN);
		priv->mac_ready = 2;
	}
	else
		priv->mac_ready = 0;

	err = register_netdev(dev);

	if (err) {
		printk(KERN_WARNING HELMUT_DEV_NAME ": error %d registering network device %s\n", 
			   err, dev->name);
		return err;
	}

	if (strcmp(priv->iport->portname, idev) || strcmp(priv->oport->portname, odev))
	{
		/* user configuration is twisted, but we're smarter so just inform him */
		printk(KERN_WARNING "%s: Using %s as RX and %s as TX port\n",
				dev->name, priv->iport->portname, priv->oport->portname);
	}

	return 0;

free:
	free_netdev(dev);

	return err;
}

/*
 * void helmut_remove_interface(minor)
 *
 * removes interface "veth<minor>"
 */
static void helmut_remove_interface(unsigned int minor)
{
	struct helmut_private *priv;
	struct net_device *dev;
	unsigned long flags;

	priv = NULL;
	dev = NULL;

	/* search list of devices for the one */
	spin_lock_irqsave(&helmut_lock, flags);
	/* scan the list ...*/
	list_for_each_entry(priv, &helmut_devlist, list)
	{
		if (priv->minor == minor)
		{
			list_del(&priv->list);
			dev = priv->dev;
			break;
		}
	}
	spin_unlock_irqrestore(&helmut_lock, flags);

	if (dev == NULL) {
		printk(KERN_INFO HELMUT_DEV_NAME ": cannot remove device (id %d not found)\n", 
			minor);
		return;
	}

	unregister_netdev(dev);
	free_netdev(dev);
}


/* --------------------------------------------------------------------- */

/*
 * /proc/vmportnet handling
 */

/* mac address decoding: string "12:34:56:78:90:ab:cd" to char[6] */
static int helmut_string_to_mac(void *mac, char *str)
{
	int i;
	int octet;
	unsigned char buf[ETH_ALEN];
	char *n;

	for (i = 0; i < ETH_ALEN; i++)
	{
		octet = simple_strtol(str, &n, 16);

		if (n == str)
			return -1;	/* bad mac string: not a number */

		if (i < ETH_ALEN-1 && (*n != ':' || *n == '\0'))
			return -2;	/* bad mac string: too short */

		if (octet < 0 || octet > 255)
			return -3;	/* bad mac string: octet outer range */

		str = n+1;		/* hop over colon */
		buf[i] = octet;
	}
	memcpy(mac, &buf, ETH_ALEN);
	return 0;			/* OK */
}


/*
 * /proc/vmportnet accepts three different strings to add and remove devices:
 *
 * "7"               - remove device veth7
 * "3 net1rx net8tx" - connect RX port "net1rx" and TX port "net8tx" as veth3
 * "0 net0rx net0tx 12:34:56:78:9a:cd" - connect ports AND setup MAC address
 */
#define TMPSIZE 128
static ssize_t config_store(struct device_driver *driver, const char *buf,
			    size_t count)
{
	char *iname;
	char *oname;
	char *macstr;
	unsigned int minor;
	unsigned char mac[ETH_ALEN];
	int ret, err;

	if (!count)
		return -EINVAL;

	if (count > TMPSIZE)
		count = TMPSIZE;

	/* allocate temp buffers */
	iname = kmalloc(TMPSIZE+1, GFP_KERNEL);
	oname = kmalloc(TMPSIZE+1, GFP_KERNEL);
	macstr = kmalloc(TMPSIZE+1, GFP_KERNEL);
	if (!iname || !oname || !macstr)
	{
		ret = -ENOMEM;
		goto free;
	}

	ret = sscanf(buf, "%u %s %s %s\n", &minor, iname, oname, macstr);
	if (ret == 4)
	{
		/* activate device with given mac address*/
		if (helmut_string_to_mac(&mac, macstr) == 0)
		{
			DBG("proc_write: activating [%d] [%s] [%s]\n", minor, iname, oname);
			err = helmut_add_interface(minor, iname, oname, &mac, 1);
			DBG("returned %d\n", err);
		}
	}
	else if (ret == 3)
	{
		/* activate device using default mac address */
		DBG("proc_write: activating [%d] [%s] [%s]\n", minor, iname, oname);
		err = helmut_add_interface(minor, iname, oname, NULL, 0);
		DBG("returned %d\n", err);
	}
	else if (ret == 1)
	{
		/* print some status ... */
		DBG("proc_write: removing [%d]\n", minor);
		helmut_remove_interface(minor);
	}
	ret = count;

free:
	kfree(macstr);
	kfree(oname);
	kfree(iname);

	return ret;
}

/*
 * 7 net9rx net9tx 12:34:56:78:9a:cd 0
 * ^   ^      ^            ^         ^
 * |   |      |            |         +-- Use counter
 * |   |      |            +------------ MAC address
 * |   |      +------------------------- TX port
 * |   +-------------------------------- RX port
 * +------------------------------------ veth<x> minor number
 *
 */
static ssize_t config_show(struct device_driver *driver, char *buf)
{
	struct helmut_private *priv;
	unsigned long flags;
	char *p = buf;

	p += sprintf(p, "# <id> <RX_port> <TX_port> <MAC> <use_counter>\n");

	/* print list of devices */
	spin_lock_irqsave(&helmut_lock, flags);
	list_for_each_entry(priv, &helmut_devlist, list) {
		p += sprintf(p, "%d %s %s %02x:%02x:%02x:%02x:%02x:%02x %d\n",
			priv->minor,
			priv->iport->portname, priv->oport->portname,
			priv->dev->dev_addr[0], priv->dev->dev_addr[1],
			priv->dev->dev_addr[2], priv->dev->dev_addr[3],
			priv->dev->dev_addr[4], priv->dev->dev_addr[5],
			priv->use_counter);
	}
	spin_unlock_irqrestore(&helmut_lock, flags);

	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RW(config);
#else
static DRIVER_ATTR(config, (S_IWUSR | S_IRUGO), config_show, config_store);
#endif

/* --------------------------------------------------------------------- */

/*
 * Module init && exit
 */

static int veth_probe(struct platform_device *pdev)
{
	struct helmut_boot_info *bi;
	unsigned int dev;
	int err;

	for (dev = 0; dev < helmut_boot_num; dev++) {
		bi = &helmut_boot_cmdline[dev];

		err = helmut_add_interface(bi->minor, bi->iname, bi->oname,
				&bi->mac, bi->mac_provided);

		if (err == 0)
		{
			printk(KERN_INFO HELMUT_DEV_NAME ": added boot inferface using %s, %s\n",
			       bi->iname, bi->oname);
		}
		else
		{
			printk(KERN_ERR HELMUT_DEV_NAME ": failed to add boot inferface using %s, %s; error %d\n",
			       bi->iname, bi->oname, err);
			return err;
		}
	}

	return 0;
}

static int veth_remove(struct platform_device *pdev)
{
	struct helmut_private *priv, *temp;

	list_for_each_entry_safe(priv, temp, &helmut_devlist, list) {
		helmut_remove_interface(priv->minor);
	}

	return 0;
}

static struct platform_driver veth_driver = {
	.driver	= {
		.name	= HELMUT_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe   = veth_probe,
	.remove = veth_remove,
};

static int __init helmut_init(void)
{
	int err;
	
	INIT_LIST_HEAD(&helmut_devlist);

	if ((err = platform_driver_register(&veth_driver)) < 0)
		return err;

	if ((err = driver_create_file(&veth_driver.driver, &driver_attr_config)) < 0)
		return err;

	helmut_platform_device = platform_device_register_simple(HELMUT_DEV_NAME, 0,
								 NULL, 0);
	if (IS_ERR(helmut_platform_device)) {
		printk(KERN_ERR HELMUT_DEV_NAME ": device registration failed\n");
		err = PTR_ERR(helmut_platform_device);
		platform_driver_unregister(&veth_driver);
		return err;
	}

	return 0;
}

module_init(helmut_init);

/*
 * Shutdown driver
 */
static void __exit helmut_exit(void)
{
	driver_remove_file(&veth_driver.driver, &driver_attr_config);

	platform_device_unregister(helmut_platform_device);
	platform_driver_unregister(&veth_driver);
}

module_exit(helmut_exit);

#ifndef MODULE

/*
 * parse veth=... kernel command-line
 *   veth=<minor>,<iname>,<oname>[,<mac>]
 *   veth=0,netrx,nettx
 */
static int __init helmut_setup(char *options)
{
	struct helmut_boot_info *bi;
	int size;
	char *n;

	if (helmut_boot_num >= MAX_BOOT_DEVICES)
		return 0;

	bi = &helmut_boot_cmdline[helmut_boot_num];

	/* get first substring: minor(int) */
	bi->minor = simple_strtol(options, &n, 0);

	if (n == options || *n != ',' || *n == '\0')
		return 0;	/* bad */

	/* hop over the comma */
	options = n+1;

	/* copy second substring: iname */
	n = bi->iname;
	size = 0;
	while (options && *options && *options != ',' && size < MAX_PORT_NAME-1)
	{
		*n++ = *options++;
		size++;
	}
	*n = 0;

	if (!size)
		return 0;	/* bad */

	/* hop over the comma */
	if (options && *options)
		options++;


	/* copy third substring: oname */
	n = bi->oname;
	size = 0;

	while (options && *options && *options != ',' && size < MAX_PORT_NAME-1)
	{
		*n++ = *options++;
		size++;
	}
	*n = 0;
	if (!size)
		return 0;	/* bad */

	/* optional fourth substring: mac address */
	if (options && *options == ',')
	{
		options++;	/* hop over the comma */
		if (helmut_string_to_mac(&bi->mac, options) < 0)
			return 0;	/* bad mac */
		bi->mac_provided = 1;
	}
	else
	{
		/* default MAC address */
		helmut_mk_special_mac(bi->mac, bi->minor);
		bi->mac_provided = 0;
	}

	/* mark bootargs as valid */
	helmut_boot_num++;

	return 0;
}

__setup("vmport_net=", helmut_setup);
#endif

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS Communication Ports (PCP) virtual network driver");
MODULE_LICENSE("GPL");
