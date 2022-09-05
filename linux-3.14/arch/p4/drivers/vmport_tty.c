/*
 *  P4VM port "tty" driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2003-2011 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/vmapi_calls.h>
#include "vmport_core.h"

#define TILLY_MAJOR		200	/* 204 conflicts with other serial ports */
#define TILLY_DEV_NAME		"vmport_tty"	/* device name */
#define PORT_VMPORT		30

#define TILLY_BOOTCONSOLE_DEFAULT_INAME "console0rx"	/* default INPUT name */
#define TILLY_BOOTCONSOLE_DEFAULT_ONAME "console0tx"	/* default OUTPUT name */

/*
 * Linux TTY code does not support a dynamic number of tty devices,
 * alloc_tty_driver() callocates driver information for a fixed number
 * of devices. We better provide a configuration option to increase
 * the number of supported devices ...
 */
#define TILLY_MAX	CONFIG_P4VM_PORT_TTY_DEVICES

struct tilly_driverinfo
{
	struct uart_port uart;

	/* internal states */
	int configured;

	/* port interface */
	struct vmport_core_portinfo *iport;
	struct vmport_core_portinfo *oport;

	struct platform_device *pdev;
};

/*
 * global variables
 */
static struct tilly_driverinfo tilly_driverinfo[TILLY_MAX];

static spinlock_t tilly_lock = __SPIN_LOCK_UNLOCKED(tilly_lock);	/* protects driver_info ... */

static struct uart_driver tilly_reg;

#ifdef CONFIG_P4VM_PORT_CONSOLE
/* boot console strings */
#define MAX_BOOTCONSOLE_NAME 32
static char tilly_bootconsole_iname[MAX_BOOTCONSOLE_NAME];
static char tilly_bootconsole_oname[MAX_BOOTCONSOLE_NAME];
static int tilly_bootconsole_port = -1;
static int tilly_bootconsole_tty = -1;	/* -1: not used, otherwise tty */
#endif /*CONFIG_P4VM_PORT_CONSOLE*/


/* --------------------------------------------------------------------- */

/*
 * IRQ callbacks (called by vmport_core IRQs)
 */


/* IRQ cb received - insert into tty ... */
/** called only when a message is ready to be read */
static void tilly_rx_int(void *data)
{
	struct tilly_driverinfo *info = (struct tilly_driverinfo *) data;
	struct uart_port *port = &info->uart;
	unsigned int ch;
	int i, size;

	size = vmapi_qport_receive(info->iport->line, info->iport->buf,
								info->iport->framesize);
	if (size < 0)
		info->iport->error = size;

	for (i = 0; i < size; i++)
	{
		port->icount.rx++;
		ch = info->iport->buf[i];
		if (uart_handle_sysrq_char(port, ch))
			continue;

		uart_insert_char(port, 0, 0, ch, 0);
	}
	tty_flip_buffer_push(&port->state->port);

	/* reactivate the thread */
	info->iport->state_ready = 0;
	vmport_core_poll_start(info->iport);
}


/* IRQ cb transmitted - refill buffers */
/** called only when a QPORT is ready to be written (free space available) */
static void tilly_tx_int(void *data)
{
	int size, count;
	struct tilly_driverinfo *info = (struct tilly_driverinfo *) data;
	struct uart_port *port = &info->uart;
 	struct circ_buf *xmit = &port->state->xmit;

	if (port->x_char) {
		/* ignore special characters */
		port->icount.tx++;
		port->x_char = 0;
		info->iport->state_ready = 0;
		vmport_core_poll_start(info->oport);
		return;
	}

	if (uart_tx_stopped(port) || uart_circ_empty(xmit))
		return;

	count = 0;
	while (count < info->oport->framesize && !uart_circ_empty(xmit)) {
		info->oport->buf[count++] = xmit->buf[xmit->tail];
		
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	/* bytes stored, send them */
	size = vmapi_qport_send(info->oport->line, info->oport->buf, count);
	if (size < 0 )
		info->oport->error = size;
	else
		info->oport->error = 0;

	/* and reactivate the thread ... */
	info->oport->state_ready = 0;
	vmport_core_poll_start(info->oport);
}

/* --------------------------------------------------------------------- */

/*
 * Add and remove link logic
 */

static int tilly_add_device(unsigned int tty, const char *idev, const char *odev)
{
	struct tilly_driverinfo *info;
	struct vmport_core_portinfo *port;
	int i, retval;

	if (tty >= TILLY_MAX)
		return -ENODEV;

	info = &tilly_driverinfo[tty];

	if (info->configured)
		return -EBUSY;

	spin_lock(&tilly_lock);
	
	/* search for ports ... */
	for (i = 0; i < vmport_core_count; i++)
	{
		port = &vmport_core_portinfo[i];
		if (strcmp(port->portname, idev) == 0 ||
			strcmp(port->portname, odev) == 0)
		{
			if (port->direction == VMPORT_CORE_DIR_RECV) {
				if (info->iport) {
					printk(KERN_WARNING TILLY_DEV_NAME ": RX port already set, ignoring %s \n", 
							port->portname);
				} else {
					info->iport = port;
				}
			}
			else if (port->direction == VMPORT_CORE_DIR_SEND) {
				if (info->oport) {
					printk(KERN_WARNING TILLY_DEV_NAME ": TX port already set, ignoring %s \n", 
							port->portname);
				} else {
					info->oport = port;
				}
			}
		}
	}
	
	if (info->iport && info->oport)
	{
		/* check if ports ARE not used ... */
		if (info->iport->count || info->oport->count)
		{
			/* device is used by other driver */
			retval = -EBUSY;
			goto fail;
		}
		
		retval = uart_add_one_port(&tilly_reg, &info->uart);
		if (retval) 
			printk(KERN_WARNING TILLY_DEV_NAME ": could not register port %d\n", retval);
		else
		{
			info->uart.fifosize = min(info->iport->framesize, info->oport->framesize);
			info->configured = 1;	/* OK, both ports are fine */
			retval = tty;
		}

	}
	else
		retval = -EBUSY;

fail:
	if (retval < 0)
	{
		info->iport = NULL;
		info->oport = NULL;
	}
	spin_unlock(&tilly_lock);
	return retval;
}

static void tilly_remove_device(unsigned int tty)
{
	struct tilly_driverinfo *info;
	int ret;

	if (tty >= TILLY_MAX)
		return;

	info = &tilly_driverinfo[tty];

	spin_lock(&tilly_lock);
	if (info->configured)
	{
		/* Warning: The device will be removed the hard way */
		info->configured = 0;
		ret = uart_remove_one_port(&tilly_reg, &info->uart);
	}
	spin_unlock(&tilly_lock);
}


/* --------------------------------------------------------------------- */

/*
 * /proc filesystem related code
 */

#define TMPSIZE 128
static ssize_t config_store(struct device_driver *driver, const char *buf,
			    size_t count)
{
	char *iname;
	char *oname;
	unsigned int ttynum;
	int ret;

	if (!count)
		return -EINVAL;

	if (count > TMPSIZE)
		count = TMPSIZE;

	/* allocate temp buffers */
	iname = kmalloc(TMPSIZE+1, GFP_KERNEL);
	oname = kmalloc(TMPSIZE+1, GFP_KERNEL);
	ret = -ENOMEM;
	if (!iname || !oname)
		goto free;

	/* fetch from userspace and append zero */
	ret = -EFAULT;

	ret = sscanf(buf, "%u %s %s\n", &ttynum, iname, oname);
	if (ret == 3)
	{
		/* activate device request */
		ret = tilly_add_device(ttynum, iname, oname);
		if(ret < 0)
			printk(KERN_WARNING TILLY_DEV_NAME ": unable to register console %d\n", ret);
	}
	else if (ret == 1)
	{
		/* remove device request */
#ifdef CONFIG_P4VM_PORT_CONSOLE
		if(ttynum == tilly_bootconsole_tty)
			printk(KERN_WARNING TILLY_DEV_NAME ": remvoing boot console not allowed\n");
		else
#endif
			tilly_remove_device(ttynum);
	}
	ret = count;

free:
	kfree(oname);
	kfree(iname);

	return ret;
}


static ssize_t config_show(struct device_driver *driver, char *buf)
{
	struct tilly_driverinfo *info;
	char *p = buf;
	int i;

	p += sprintf(p, "# <minor> <RX_port> <TX_port>\n");

	for (i = 0; i < TILLY_MAX; i++)
	{
		info = &tilly_driverinfo[i];

		spin_lock(&tilly_lock);
		if (info->configured)
			p += sprintf(p, "%d %s %s\n", i, info->iport->portname, info->oport->portname);
		spin_unlock(&tilly_lock);
	}

	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RW(config);
#else
static DRIVER_ATTR(config, (S_IWUSR | S_IRUGO), config_show, config_store);
#endif

/* --------------------------------------------------------------------- */

static int tilly_startup(struct uart_port *port)
{
	int ret;
	unsigned int nb_messages, max_messages;
	struct tilly_driverinfo *info = port->private_data;

	/* Called when the FIRST application opens the port */
	ret = vmport_core_stat_num_msg(info->iport, &nb_messages, &max_messages);
	if (ret != 0)
		return ret;

	/* enable RX thread and "hand-feed" any pending data into tty layer */
	if (nb_messages > 0)
		tilly_rx_int(info);
	else
		vmport_core_poll_start(info->iport);

	return 0;
}

static void tilly_shutdown(struct uart_port *port)
{
	struct tilly_driverinfo *info = port->private_data;

	/* Called when the LAST application closes the port. */
	vmport_core_poll_stop(info->iport);
	vmport_core_poll_stop(info->oport);
}

static void tilly_start_tx(struct uart_port *port)
{ 
	struct tilly_driverinfo *info = port->private_data;

	vmport_core_poll_start(info->oport);
}

static const char *tilly_type(struct uart_port *port)
{
	return (port->type == PORT_VMPORT) ? "VMPORT_TTY" : NULL;
}

static int tilly_request_port(struct uart_port *port)
{
	struct tilly_driverinfo *info = port->private_data;
	int err;

	err = vmport_core_init_port(info->iport, tilly_rx_int, info,
								VMPORT_CORE_TYPE_TTY, NULL, 0);
	if (err)
		return err;

	err = vmport_core_init_port(info->oport, tilly_tx_int, info,
								VMPORT_CORE_TYPE_TTY, NULL, 0);
	if (err)
		vmport_core_exit_port(info->iport);

	return err;
}

static void tilly_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_VMPORT;
		tilly_request_port(port);
	}
}

static void tilly_release_port(struct uart_port *port)
{
	struct tilly_driverinfo *info = port->private_data;

	vmport_core_exit_port(info->iport);
	vmport_core_exit_port(info->oport);
	info->iport = NULL;
	info->oport = NULL;
}

static unsigned int tilly_tx_empty(struct uart_port *port)
{
	unsigned int nb_messages, max_messages;
	struct tilly_driverinfo *info = port->private_data;

	vmport_core_stat_num_msg(info->oport, &nb_messages, &max_messages);

	return (nb_messages == max_messages) ? TIOCSER_TEMT : 0;
}

static void tilly_set_mctrl(struct uart_port *port, u_int mctrl)
{
	/* handshakes not supported */
}

static unsigned int tilly_get_mctrl(struct uart_port *port)
{
	/* modem status lines not supported */
	return 0;
}

static void tilly_stop_tx(struct uart_port *port)
{
	struct tilly_driverinfo *info = port->private_data;

	/* disable output thread */
	vmport_core_poll_stop(info->oport);
}

static void tilly_stop_rx(struct uart_port *port)
{
	struct tilly_driverinfo *info = port->private_data;
	
	/* disable input thread */
	vmport_core_poll_stop(info->iport);
}

static void tilly_enable_ms(struct uart_port *port)
{
	/* modem status interrupt not supported */
}

static void tilly_break_ctl(struct uart_port *port, int break_state)
{
	/* break signal not supported */
}

static void tilly_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old)
{
	unsigned int baud;

	/* ignore parameters, but set some internal variables */
	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk / 16);
	uart_update_timeout(port, termios->c_cflag, baud);

	port->read_status_mask = 0;
	port->ignore_status_mask = 0;
}

/* --------------------------------------------------------------------- */

/*
 * Hook table
 */
static struct uart_ops tilly_ops = {
	.startup	= tilly_startup,
	.shutdown	= tilly_shutdown,
	.start_tx	= tilly_start_tx,
	.type		= tilly_type,
	.config_port	= tilly_config_port,
	.request_port	= tilly_request_port,
	.release_port	= tilly_release_port,
	.tx_empty	= tilly_tx_empty,
	.set_mctrl	= tilly_set_mctrl,
	.get_mctrl	= tilly_get_mctrl,
	.stop_tx	= tilly_stop_tx,
	.stop_rx	= tilly_stop_rx,
	.enable_ms	= tilly_enable_ms,
	.break_ctl	= tilly_break_ctl,
	.set_termios	= tilly_set_termios,
};

/*
 * Configure the port from the platform device resource info.
 */
static void tilly_init_ports(void)
{
	static int first = 1;
	struct uart_port *port;
	int i;

	if (!first)
		return;
	first = 0;

	for (i = 0; i < TILLY_MAX; i++)
	{
		port = &tilly_driverinfo[i].uart;
		
		port->iotype	= UPIO_MEM;
		port->flags	= UPF_BOOT_AUTOCONF;
		port->ops	= &tilly_ops;
		port->fifosize	= 32;
		port->line	= i;
		port->mapbase	= 1;	/* must be set */
		port->private_data = &tilly_driverinfo[i];
		port->uartclk	= 115200 * 16;
	}
}


/* --------------------------------------------------------------------- */

#ifdef CONFIG_P4VM_PORT_CONSOLE

/*
 * boot console stuff
 */
static void tilly_console_write(struct console *c, const char *s, unsigned count)
{
	unsigned sent;

	sent = 0;
	while (sent < count) {
		char tmp[128];
		unsigned i;
		/* build single chunk to send, extend \n to \r\n
		 * i is index in tmp
		 */
		i = 0;
		while ((i < sizeof(tmp)-1) && (sent < count)) {
			if (s[sent] == '\n')
			{
				tmp[i] = '\r';
				i++;
			}
			tmp[i] = s[sent];
			i++;
			sent++;
		}

		/* ignore errors:
		 * messages might get lost, but we don't care during early boot
		 * Warning: This function can block
		 */
		vmapi_qport_send(tilly_bootconsole_port, tmp, i);
	}
}

static int __init tilly_console_setup(struct console *c, char *options)
{
	struct vmport_core_portinfo *port;
	char *n;
	int size;
	int i;

	if (c->index < 0 || c->index >= TILLY_MAX)
	{
		return -ENODEV;
	}

	/* copy first substring: iname */
	n = tilly_bootconsole_iname;
	size = 0;
	while (options && *options && *options != ',' && size < MAX_BOOTCONSOLE_NAME-1)
	{
		*n++ = *options++;
		size++;
	}
	*n = 0;
	if (!size)
		strcpy(tilly_bootconsole_iname, TILLY_BOOTCONSOLE_DEFAULT_INAME);

	/* hop over the comma */
	if (options && *options)
		options++;

	/* copy second substring: oname */
	n = tilly_bootconsole_oname;
	size = 0;
	while (options && *options && *options != ',' && size < MAX_BOOTCONSOLE_NAME-1)
	{
		*n++ = *options++;
		size++;
	}
	*n = 0;
	if (!size)
		strcpy(tilly_bootconsole_oname, TILLY_BOOTCONSOLE_DEFAULT_ONAME);

	/* find boot console ... */
	for (i = 0; i < vmport_core_count; i++)
	{
		port = &vmport_core_portinfo[i];
		if (!strcmp(port->portname, tilly_bootconsole_oname) && port->direction == VMPORT_CORE_DIR_SEND)
		{
			tilly_bootconsole_port = port->line;
			break;
		}
		else if (!strcmp(port->portname, tilly_bootconsole_iname) && port->direction == VMPORT_CORE_DIR_SEND)
		{
			/* input/output twisted */
			tilly_bootconsole_port = port->line;
		}
	}
	if (tilly_bootconsole_port == -1)
	{
		vmapi_cprintf(TILLY_DEV_NAME ": vmports %s, %s for boot console not found\n", tilly_bootconsole_iname, tilly_bootconsole_oname);
		return -ENODEV;
	}

	tilly_bootconsole_tty = c->index;

	return uart_set_options(&tilly_driverinfo[c->index].uart,
							c, 115200, 'n', 8, 'n');
}

static struct console tilly_console = {
	.name		= "ttyMP",
	.write		= tilly_console_write,
	.device		= uart_console_device,
	.setup		= tilly_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &tilly_reg,
};

static int __init tilly_console_init(void)
{
	tilly_init_ports();
	register_console(&tilly_console);

	return 0;
}

/* console_initcall is too early because we need configured vmports */
postcore_initcall(tilly_console_init);

#define TILLY_CONSOLE_DEVICE	&tilly_console
#else
#define TILLY_CONSOLE_DEVICE	NULL
#endif /*CONFIG_P4VM_PORT_CONSOLE*/

static struct uart_driver tilly_reg = {
	.owner			= THIS_MODULE,
	.driver_name		= TILLY_DEV_NAME,
	.dev_name		= "ttyMP",
	.major			= TILLY_MAJOR,
	.minor			= 0,
	.nr			= TILLY_MAX,
	.cons			= TILLY_CONSOLE_DEVICE,
};

static struct platform_driver tilly_serial_driver = {
	.driver		= {
		.name	= TILLY_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

/* --------------------------------------------------------------------- */

/*
 * Module loading code
 */


/*
 * int tilly_init(void)
 *
 * Initialize and register driver
 */
static int __init tilly_init(void)
{
	int ret, i;

	tilly_init_ports();

	ret = uart_register_driver(&tilly_reg);
	if (ret)
		return ret;

	for (i = 0; i < TILLY_MAX; i++)
		tilly_driverinfo[i].pdev =
			platform_device_register_simple(TILLY_DEV_NAME, i, NULL,
							0);

	ret = platform_driver_register(&tilly_serial_driver);
	if (ret)
	{
		uart_unregister_driver(&tilly_reg);
		return ret;
	}

	ret = driver_create_file(&tilly_serial_driver.driver, &driver_attr_config);
	if (ret)
		return ret;

#ifdef CONFIG_P4VM_PORT_CONSOLE
	if (tilly_bootconsole_tty != -1)
	{
		printk(KERN_INFO TILLY_DEV_NAME ": registering boot console: %d, %s, %s\n", tilly_bootconsole_tty, tilly_bootconsole_iname, tilly_bootconsole_oname);
		if (tilly_add_device(tilly_bootconsole_tty, tilly_bootconsole_iname, tilly_bootconsole_oname)<0)
		{
			printk(KERN_WARNING TILLY_DEV_NAME ": unable to register boot console\n");
		}
	}
#endif /*CONFIG_P4VM_PORT_CONSOLE*/

	printk(KERN_INFO TILLY_DEV_NAME ": driver registered as major %d\n", TILLY_MAJOR);
	return 0;
}

/*
 * void tilly_exit(void)
 *
 * Shutdown driver
 */
static void __exit tilly_exit(void)
{
	int i;

	driver_remove_file(&tilly_serial_driver.driver, &driver_attr_config);

	for (i = 0; i < TILLY_MAX; i++) {
		if (tilly_driverinfo[i].configured)
			tilly_remove_device(i);

		platform_device_unregister(tilly_driverinfo[i].pdev);
	}
	/* first remove the device, then it is safe to unregister the uart
	 * driver
	 */
	uart_unregister_driver(&tilly_reg);
	platform_driver_unregister(&tilly_serial_driver);
}

module_init(tilly_init);
module_exit(tilly_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS Communication Ports (PCP) TTY driver");
MODULE_LICENSE("GPL");
