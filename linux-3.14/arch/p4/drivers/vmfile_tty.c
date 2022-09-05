/*
 *  P4VM file provider based "tty" driver
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 *          David Engraf <den@sysgo.com>
 * 
 *  Copyright (C) 2004-2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/module.h>
#include <linux/console.h>
#include <linux/interrupt.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/version.h>
#include <asm/irq.h>
#include <asm/kernthreads.h>
#include <asm/uaccess.h>
#include <asm/vmapi_calls.h>
#include <asm/locking.h>


#define FUFU_MAJOR     201
#define FUFU_DEV_NAME  "vmfile_tty"     /* device name    */
#define PORT_VMFILE	31

/*
 * Linux TTY code does not support a dynamic number of tty devices,
 * alloc_tty_driver() callocates driver information for a fixed number
 * of devices. We better provide a configuration option to increase
 * the number of supported devices ...
 */
#define FUFU_MAX	CONFIG_P4VM_FILE_TTY_DEVICES

struct fufu_linkinfo
{
	/* low level thread related */
	unsigned int irq;

	/* states part */
	volatile int state_active;
	struct uk_uevent irq_sleep;
	int error;

	/* buffer */
	int inbuf;
	int outbuf;
	unsigned char *buf;
};

#define TX_BUF_SIZE PAGE_SIZE
#define RX_BUF_SIZE PAGE_SIZE

struct fufu_driverinfo
{
	struct platform_device *device;
	struct uart_port	uart;	/* uart */

	/* internal states */
	char *filename;
	int handle;
	int framesize;

	/* thread interface */
	struct fufu_linkinfo rx;
	struct fufu_linkinfo tx;

	unsigned short msr;	/* last msr */
	unsigned short new_msr;	/* new msr, read by IRQ */
	u_int startup_mctrl;
};


/*
 * global variables
 */
static struct fufu_driverinfo fufu_driverinfo[FUFU_MAX];

static DEFINE_MUTEX(fufu_lock);	/* protects driver_info ... */

#ifdef CONFIG_P4VM_FILE_CONSOLE
/* boot console strings */
#define MAX_BOOTCONSOLE_NAME 128
static char fufu_bootconsole_filename[MAX_BOOTCONSOLE_NAME];
static int fufu_bootconsole_tty = -1;	/* -1: not used, otherwise tty */
static struct fufu_driverinfo *fufu_bootconsole_info = NULL;
static int fufu_bootconsole_handle = -1;
static unsigned int fufu_bootconsole_size;
#endif /*CONFIG_P4VM_FILE_CONSOLE*/


static struct uart_driver fufu_uart;

static void fufu_stop_tx(struct uart_port *port);
static void fufu_stop_rx(struct uart_port *port);

static int fufu_try_open(struct fufu_driverinfo *info)
{
	int handle;
	vmapi_off_t size_o;
	unsigned int size;

	handle = vmapi_file_open4(info->filename, VMAPI_O_R | VMAPI_O_W, &size_o, NULL);
	size = size_o;
	if (handle >= 0) {
		/* open succeeded */
		/* romfiles etc don't have a size, so fake one */
		if (size == 0)
			size = TX_BUF_SIZE;
		info->handle = handle;
		info->framesize = min(size, (unsigned int)TX_BUF_SIZE);
	}

	return handle;
}

static void handle_modem_status(struct fufu_driverinfo *info)
{
	struct uart_port *port = &info->uart;
	unsigned int status_change;
	
	status_change = info->new_msr ^ info->msr;

	if (status_change & TIOCM_CAR)
		uart_handle_dcd_change(port, info->new_msr & TIOCM_CAR);

	if (status_change & TIOCM_DSR)
		port->icount.dsr++;

	if (status_change & TIOCM_RNG)
		port->icount.rng++;

	/* CTS is completely handled by the File Provider. From the personality
	 * point of view, we are always allowed to write...
	 */

	wake_up_interruptible(&port->state->port.delta_msr_wait);

	info->msr = info->new_msr;
}

/* --------------------------------------------------------------------- */

/*
 * RX state machine:
 *
 * The RX receiver logic consists of an endless reading async thread
 * triggering an interrupt to process the read data.
 *
 */


/*
 * void fufu_thread(unsigned int irq, void *dev)
 *
 * is irq dispatcher thread and runs in P4 thread context
 *
 * NOTE: do not call any linux functions, as
 *       this code is not executed in linux context!
 */
static void fufu_thread(unsigned int irq, void *dev)
{
	struct fufu_driverinfo *info = dev;
	struct fufu_linkinfo *link;
	int tx, pos, size;

	if (irq == info->tx.irq)
	{
		link = &info->tx;
		tx = 1;
	}
	else
	{
		link = &info->rx;
		tx = 0;
	}

	while (1)
	{
		while (!link->state_active)
		{
			/* sleep until woken */
			sleep_in_irq_thread(&link->irq_sleep);
		}

		if (tx && link->inbuf)
		{
			/* send msg and handle partial send */
			for (pos = 0, size = link->inbuf; size;) {
				link->outbuf = vmapi_file_write(info->handle, &link->buf[pos], size);
				if (link->outbuf <= 0)
					break;

				size -= link->outbuf;
				pos  += link->outbuf;
			}
			if (link->outbuf < 0)
			{
				link->error  = link->outbuf;	/* contains error code */
				link->outbuf = -1;
			}
			else
			{
				if (link->outbuf == 0)
				{
					/* 
					 * Calling vmapi_get_mstat in the linux interrupt function results
					 * in a BUG(), so call it here.
					 */
					vmapi_get_mstat(info->handle, &info->new_msr);
				}

				link->error = 0; /* sucessfully send */
			}
		}
		else if (!tx)
		{
			/* receive msg from thread */
			link->inbuf = vmapi_file_read(info->handle, link->buf, info->framesize);
			if (link->inbuf < 0)
			{
				link->error = link->inbuf;
				link->inbuf = 0;
			}
			else
			{
				if (link->inbuf == 0)
				{
					/* 
					 * Calling vmapi_get_mstat in the linux interrupt function results
					 * in a BUG(), so call it here.
					 */
					vmapi_get_mstat(info->handle, &info->new_msr);
				}

				link->error = 0;	/* indicates: NO ERROR */
			}
		}

		/* we got an interrupt, now call the handler */
		do_IRQ(irq);
	}
}

/*
 * void fufu_rx_irq(int irq, void *dev, struct pt_regs *regs)
 *
 * called by do_IRQ when buffer received
 */
static irqreturn_t fufu_rx_irq(int irq, void *dev)
{
	struct fufu_driverinfo *info = dev;
	struct uart_port *port = &info->uart;
	int i;
	unsigned int ch;

	/* disarm thread */
	info->rx.state_active = 0;

	if (info->rx.inbuf) {
		for (i = 0; i < info->rx.inbuf; i++)
		{
			port->icount.rx++;
			ch = info->rx.buf[i];
			if (uart_handle_sysrq_char(port, ch))
				continue;

			uart_insert_char(port, 0, 0, ch, 0);
		}
		tty_flip_buffer_push(&port->state->port);
		info->rx.inbuf = 0;
	} else {
		handle_modem_status(info);
	}

	/* reactivate the thread */
	info->rx.state_active = 1;

	return IRQ_HANDLED;
}

/* --------------------------------------------------------------------- */

/*
 * TX state machine:
 *
 */


/*
 * void fufu_tx_irq(int irq, void *dev, struct pt_regs *regs)
 *
 * called by do_IRQ when buffer was transmitted
 */
static irqreturn_t fufu_tx_irq(int irq, void *dev)
{
	struct fufu_driverinfo *info = dev;
	struct uart_port *port = &info->uart;
	struct circ_buf *xmit = &port->state->xmit;

	/* disarm thread */
	info->tx.state_active = 0;

	/* nothing written, so check modem status lines */
	if (info->tx.outbuf == 0 && info->tx.inbuf) {
		handle_modem_status(info);
	} else {

		if (info->tx.inbuf)
		{
			/* remove written chars from the ring buffer */
			info->tx.inbuf = 0;
		}

		if (port->x_char) {
			/* ignore */
			port->icount.tx++;
			port->x_char = 0;
			goto out;
		}

		if (uart_tx_stopped(port) || uart_circ_empty(xmit)) {
			/* stopped or no more bytes -> leave state_active on 0 */
			goto out;
		}

		while (!uart_circ_empty(xmit) &&
			info->tx.inbuf < info->framesize) {
			info->tx.buf[info->tx.inbuf++] = xmit->buf[xmit->tail];
			
			xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
			port->icount.tx++;
		}

		if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
			uart_write_wakeup(port);

	}

	/* reactivate the thread ... */
	if (info->tx.inbuf)
		info->tx.state_active = 1;

out:
	return IRQ_HANDLED;
}

/* --------------------------------------------------------------------- */

/*
 * Open / close logic
 */

static int fufu_get_irqs(struct fufu_driverinfo *info)
{
	int rx_irq;
	int tx_irq;
	int err = -ENODEV;

	rx_irq = get_free_async_irq();
	tx_irq = get_free_async_irq();

	if (!rx_irq || !tx_irq) {
		printk(KERN_ERR FUFU_DEV_NAME ": unable to get free irqs\n");
		goto release_irqs;
	}

	/* allocate RX IRQ */
	if (request_irq(rx_irq, fufu_rx_irq, 0, "vmfile_read", info)) {
		printk(KERN_ERR FUFU_DEV_NAME ": unable to request read-IRQ %d\n", rx_irq);
		goto release_irqs;
	}
	info->rx.irq = rx_irq;

	/* allocate TX IRQ */
	if (request_irq(tx_irq, fufu_tx_irq, 0, "vmfile_write", info)) {
		printk(KERN_ERR FUFU_DEV_NAME ": unable to request write-IRQ %d\n", tx_irq);
		goto free_rx;
	}
	info->tx.irq = tx_irq;

	/* restart IRQ threads with proper callback */
	err = enable_irq_thread(info->rx.irq, "vmfile_tty_rx", fufu_thread, info);
	if (err) {
		printk(KERN_ERR FUFU_DEV_NAME ": could not start async IRQ RX thread\n");
		err = -ENOMEM;
		goto free_tx;
	}
	
	err = enable_irq_thread(info->tx.irq, "vmfile_tty_tx", fufu_thread, info);
	if (err) {
		disable_irq_thread(info->rx.irq);

		printk(KERN_ERR FUFU_DEV_NAME ": could not start async IRQ TX thread\n");
		err = -ENOMEM;
		goto free_tx;
	}

	info->uart.irq = tx_irq;
	return 0;

free_tx:
	free_irq(info->tx.irq, info);

free_rx:
	free_irq(info->rx.irq, info);

release_irqs:
	if (rx_irq)
		release_async_irq(rx_irq);
	if (tx_irq)
		release_async_irq(tx_irq);

	return err;
}

/* --------------------------------------------------------------------- */

/*
 * Add and remove logic
 */

static int fufu_add_device(unsigned int tty, char *filename)
{
	struct fufu_driverinfo *info;
	int err;

	if (tty >= FUFU_MAX)
		return -ENODEV;

	err = -EBUSY;

	mutex_lock(&fufu_lock);

	info = &fufu_driverinfo[tty];
	if (info->filename)
		goto out_free_lock;

	/* alloc space for filename */
	info->filename = filename;

	uart_add_one_port(&fufu_uart, &info->uart);
	err = tty;

out_free_lock:
	mutex_unlock(&fufu_lock);

	return err;
}

static void fufu_remove_device(unsigned int tty)
{
	struct fufu_driverinfo *info;

	if (tty >= FUFU_MAX)
		return;

	info = &fufu_driverinfo[tty];

	mutex_lock(&fufu_lock);
	if (info->filename)
	{
		/*
		 * Remove the device the hard way, this will
		 * also call the shutdown function.
		 */
		uart_remove_one_port(&fufu_uart, &info->uart);

		kfree(info->filename);
		info->filename = NULL;
	}
	mutex_unlock(&fufu_lock);
}


/* --------------------------------------------------------------------- */

/*
 * sysfs related code
 */
#define TMPSIZE 128
static ssize_t config_store(struct device_driver *driver, const char *buf,
			    size_t count)
{
	char *filename;
	unsigned int ttynum;
	int ret;

	if (!count)
		return -EINVAL;

	if (count > TMPSIZE)
		count = TMPSIZE;

	/* allocate temp buffers */
	filename = kmalloc(TMPSIZE+1, GFP_KERNEL);
	if (!filename)
	{
		ret = -ENOMEM;
		goto free;
	}

	ret = sscanf(buf, "%u %s\n", &ttynum, filename);
	if (ret == 2)
	{
		/* activate device */
		if (fufu_add_device(ttynum, filename) >= 0)
		{
			/* OK, device added */
			/* must not free filename! */
			return count;
		}
	}
	else if (ret == 1)
	{
		/* remove device request */
#ifdef CONFIG_P4VM_FILE_CONSOLE
		if(ttynum == fufu_bootconsole_tty)
			printk(KERN_WARNING FUFU_DEV_NAME ": remvoing boot console not allowed\n");
		else
#endif
			fufu_remove_device(ttynum);
		return count;
	}
 	ret = -EINVAL;

free:
	kfree(filename);

	return ret;
}

static ssize_t config_show(struct device_driver *driver, char *buf)
{
	struct fufu_driverinfo *info;
	char *p = buf;
	int i;

	p += sprintf(p, "# <minor> <filename>\n");

	for (i = 0; i < FUFU_MAX; i++)
	{
		info = &fufu_driverinfo[i];

		mutex_lock(&fufu_lock);
		if (info->filename)
			p += sprintf(p, "%d %s\n", i, info->filename);
		mutex_unlock(&fufu_lock);
	}

	return strlen(buf);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0)
static DRIVER_ATTR_RW(config);
#else
static DRIVER_ATTR(config, (S_IWUSR | S_IRUGO), config_show, config_store);
#endif

/* --------------------------------------------------------------------- */

static void fufu_shutdown(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	/* set stop flag for rx and tx thread */
	fufu_stop_tx(&info->uart);
	fufu_stop_rx(&info->uart);

	/*
	 * Killing the rx/tx thread should be no problem
	 * even if the read/write function is blocking.
	 * Normally the serial_core stops the rx thread and
	 * waits until all Bytes were send.
	 */
	disable_irq_thread(info->rx.irq);
	disable_irq_thread(info->tx.irq);
	free_irq(info->rx.irq, info);
	free_irq(info->tx.irq, info);

	release_async_irq(info->rx.irq);
	release_async_irq(info->tx.irq);

	/*
	 * Calling file close while the threads are running results
	 * in a deadlock for the other file provders. Close the file
	 * as the last step.
	 */
	if (info->handle >= 0)
	{
#ifdef CONFIG_P4VM_FILE_CONSOLE
		if (info->handle == fufu_bootconsole_handle)
		{
			/* workaround vm_close must be called for bootconsole too, else the 
			 * RX thread will not correctly destroyed and reopening the port results
			 * in no more received bytes. Check with an init program which closes
			 * stdin/out/err like upstart.
			 */
			fufu_bootconsole_handle = -1;
			
			vmapi_file_close(info->handle);
			fufu_bootconsole_handle =
				vmapi_file_open4(fufu_bootconsole_filename,
						VMAPI_O_R | VMAPI_O_W, NULL, NULL);

			if (fufu_bootconsole_handle < 0) {
				vmapi_cprintf(FUFU_DEV_NAME ": Failed to reopen boot console <%s> %d\n",
					fufu_bootconsole_filename, fufu_bootconsole_handle);
			}
		}
		else
#endif
			vmapi_file_close(info->handle);
		info->handle = -1;
	}
}

static void fufu_start_tx(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	info->tx.state_active = 1;
	wake_irq_thread(&info->tx.irq_sleep);
}

static const char *fufu_type(struct uart_port *port)
{
	return (port->type == PORT_VMFILE) ? "VMFILE_TTY" : NULL;
}

static int fufu_request_port(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	info->tx.buf = kmalloc(TX_BUF_SIZE, GFP_KERNEL);
	info->rx.buf = kmalloc(RX_BUF_SIZE, GFP_KERNEL);
	if (!info->tx.buf || !info->rx.buf)
		panic(FUFU_DEV_NAME ": couldn't allocate free memory");

	return 0;
}

static void fufu_release_port(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	kfree(info->rx.buf);
	kfree(info->tx.buf);
}

static void fufu_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_VMFILE;
		fufu_request_port(port);
	}
}

static unsigned int fufu_tx_empty(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	return (!info->tx.inbuf) ? TIOCSER_TEMT : 0;
}

static void fufu_set_mctrl(struct uart_port *port, u_int mctrl)
{
	struct fufu_driverinfo *info = port->private_data;

	if (info->handle >= 0) {
		vmapi_set_mctrl(info->handle, mctrl);
	} else {
		/* uart not yet opened, just save mctrl and set it on open */
		info->startup_mctrl = mctrl;
	}
}

static unsigned int fufu_get_mctrl(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;
	unsigned short mstat;
	int ret;

	/* read DCD, CTS, DSR and RI */
	ret = vmapi_get_mstat(info->handle, &mstat);
	if (ret)
		mstat = 0;

	return mstat;
}

static void fufu_stop_tx(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	/* disable output thread */
	info->tx.state_active = 0;
	wake_irq_thread(&info->tx.irq_sleep);
}

static void fufu_stop_rx(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;

	/* disable input thread */
	info->rx.state_active = 0;
	wake_irq_thread(&info->rx.irq_sleep);
}

static void fufu_enable_ms(struct uart_port *port)
{
	/* 
	 * This function should activate the interrupt for
	 * the modem status. We don't support it here because
	 * it depends on the PikeOS driver whether the file
	 * provider supports handshake or not.
	*/
}

static void fufu_break_ctl(struct uart_port *port, int break_state)
{
	/* break signal not supported */
}

static void fufu_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old)
{
	struct fufu_driverinfo *info = port->private_data;
	unsigned int baud;

	/* set termios and internal variables used for timeout handling */
	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk / 16);
	uart_update_timeout(port, termios->c_cflag, baud);

	port->read_status_mask = 0;
	port->ignore_status_mask = 0;

	vmapi_set_termios(info->handle, termios->c_cflag);
}

static int fufu_startup(struct uart_port *port)
{
	struct fufu_driverinfo *info = port->private_data;
	int ret;

	/* Called when the FIRST application opens the port */

#ifdef CONFIG_P4VM_FILE_CONSOLE
	if (port->cons->index == port->line)
	{
		info->handle = fufu_bootconsole_handle;

		/* shutdown for bootconsole was called and not yet
		 * a new kernel message was printed -> reopen port
		 */
		if (info->handle == -1)
		{
			ret = fufu_try_open(info);
			if (info->handle >= 0 && fufu_bootconsole_handle == -1)
				fufu_bootconsole_handle = info->handle;
		} else {
			ret = 0;
		}
	}
	else
#endif
		ret = fufu_try_open(info);
	if (ret < 0) {
		/* unfortunatelly it is not passed to the application, instead
		 * -EIO is used
		 */
		return ret;
	}

	/* file opened -> set pre saved mctrl */
	fufu_set_mctrl(port, info->startup_mctrl);
	info->msr = 0;

	/* clear sleep state so threads will not be woken up unitialized */
	uk_uevent_init(&info->rx.irq_sleep);
	uk_uevent_init(&info->tx.irq_sleep);

	/* start RX thread in listening mode */
	info->rx.state_active = 1;
	info->tx.state_active = 0;
	wake_irq_thread(&info->rx.irq_sleep);

	/* remove pending bytes */
	info->tx.inbuf = 0;

	return fufu_get_irqs(info);
}

/* --------------------------------------------------------------------- */

/*
 * Module loading code
 */

/*
 * Hook table
 */
static struct uart_ops fufu_ops = {
	.startup	= fufu_startup,
	.shutdown	= fufu_shutdown,
	.start_tx	= fufu_start_tx,
	.type		= fufu_type,
	.config_port	= fufu_config_port,
	.request_port	= fufu_request_port,
	.release_port	= fufu_release_port,
	.tx_empty	= fufu_tx_empty,
	.set_mctrl	= fufu_set_mctrl,
	.get_mctrl	= fufu_get_mctrl,
	.stop_tx	= fufu_stop_tx,
	.stop_rx	= fufu_stop_rx,
	.enable_ms	= fufu_enable_ms,
	.break_ctl	= fufu_break_ctl,
	.set_termios	= fufu_set_termios,
};

#ifdef CONFIG_P4VM_FILE_CONSOLE
static struct console fufu_console;

#define FUFU_CONSOLE_DEVICE	&fufu_console
#else
#define FUFU_CONSOLE_DEVICE	NULL
#endif

static struct uart_driver fufu_uart = {
	.owner		= THIS_MODULE,
	.driver_name	= FUFU_DEV_NAME,
	.dev_name	= "ttyFP",
	.major		= FUFU_MAJOR,
	.minor		= 0,
	.nr		= FUFU_MAX,
	.cons		= FUFU_CONSOLE_DEVICE,
};

/*
 * Configure the port from the platform device resource info.
 */
static void fufu_init_ports(void)
{
	static int first = 1;
	struct uart_port *port;
	int i;

	if (!first)
		return;
	first = 0;

	for (i = 0; i < FUFU_MAX; i++)
	{
		port = &fufu_driverinfo[i].uart;
		
		port->iotype	= UPIO_MEM;
		port->flags	= UPF_BOOT_AUTOCONF;
		port->ops	= &fufu_ops;
		port->fifosize	= 32;
		port->line	= i;
		port->mapbase	= 1;	/* must be set */
		port->private_data = &fufu_driverinfo[i];
		port->uartclk	= 115200 * 16;

		fufu_driverinfo[i].handle = -1;
	}
}

static struct platform_driver fufu_serial_driver = {
	.driver		= {
		.name	= FUFU_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

/*
 * int fufu_init(void)
 *
 * Initialize and register driver
 */
static int __init fufu_init(void)
{
	int i, ret;

	fufu_init_ports();

	ret = uart_register_driver(&fufu_uart);
	if (ret)
		return ret;

	for (i = 0; i < FUFU_MAX; i++)
		fufu_driverinfo[i].device = platform_device_register_simple(FUFU_DEV_NAME, i, NULL, 0);
	
	ret = platform_driver_register(&fufu_serial_driver);
	if (ret)
		goto err_dev;

	ret = driver_create_file(&fufu_serial_driver.driver, &driver_attr_config);
	if (ret)
		goto err_plat;

	printk(KERN_INFO FUFU_DEV_NAME ": driver registered, %d lines available\n", FUFU_MAX);

#ifdef CONFIG_P4VM_FILE_CONSOLE
	if (fufu_bootconsole_tty != -1)
	{
		if (fufu_add_device(fufu_bootconsole_tty, fufu_bootconsole_filename)<0)
		{
			printk(KERN_WARNING FUFU_DEV_NAME ": unable to register boot console\n");
		}
		else
		{
			fufu_bootconsole_info = &fufu_driverinfo[fufu_bootconsole_tty];

			/* now fake an open request */
			fufu_bootconsole_info->handle = fufu_bootconsole_handle;
			fufu_bootconsole_info->framesize = fufu_bootconsole_size;
		}
	}

#endif /*CONFIG_P4VM_FILE_CONSOLE*/
	return 0;

err_plat:
	platform_driver_unregister(&fufu_serial_driver);
err_dev:
	for (i = 0; i < FUFU_MAX; i++)
		platform_device_unregister(fufu_driverinfo[i].device);
	uart_unregister_driver(&fufu_uart);

	return ret;
}

/*
 * void fufu_exit(void)
 *
 * Shutdown driver
 */
static void __exit fufu_exit(void)
{
	int i;

	for (i = 0; i < FUFU_MAX; i++)
		platform_device_unregister(fufu_driverinfo[i].device);

	uart_unregister_driver(&fufu_uart);
	driver_remove_file(&fufu_serial_driver.driver, &driver_attr_config);
	platform_driver_unregister(&fufu_serial_driver);
}

module_init(fufu_init);
module_exit(fufu_exit);

MODULE_AUTHOR("Alex Zuepke <azu@sysgo.de>");
MODULE_DESCRIPTION("PikeOS File Provider (client) TTY driver");
MODULE_LICENSE("GPL");


/* --------------------------------------------------------------------- */


#ifdef CONFIG_P4VM_FILE_CONSOLE
/*
 * kernel boot console code
 */

static struct fufu_console_private {
	char *buf;
	volatile int count;
	struct uk_uevent irq_sleep;
	int tx_irq;
} fufu_console_priv;

/*
 * void fufu_console_tx_thread(unsigned int irq, void *data)
 *
 * output message in P4 thread context (vmapi_file_write may block)
 *
 */
static void fufu_console_tx_thread(unsigned int irq, void *data)
{
	struct fufu_console_private *priv = data;
	int size, pos, written;

	while (1)
	{
		sleep_in_irq_thread(&priv->irq_sleep);
		for (pos = 0, size = priv->count; size;) {
			written = vmapi_file_write(fufu_bootconsole_handle,
					 &priv->buf[pos], size);
			if (written <= 0) {
				/* error condition */
				vmapi_cprintf(FUFU_DEV_NAME ": Failed to write on <%s> %d\n",
					      fufu_bootconsole_filename, written);
				break;
			}
			
			size -= written;
			pos  += written;
		}
		priv->count = 0;
	}
}

static void fufu_console_write(struct console *c, const char *s, unsigned count)
{
	unsigned sent;

	sent = 0;
	while (sent < count) {
		unsigned i;
		/* build single chunk to send, extend \n to \r\n
		 * i is index in tmp
		 */
		i = 0;
		while ((i < fufu_bootconsole_size-1) && (sent < count)) {
			if (s[i] == '\n')
			{
				fufu_console_priv.buf[i] = '\r';
				i++;
			}
			fufu_console_priv.buf[i] = s[sent];
			i++;
			sent++;
		}

		/* delegate message to TX thread */
		fufu_console_priv.count = i;
		wake_irq_thread(&fufu_console_priv.irq_sleep);

		/* do a busy wait until all bytes have been transferred */
		while(fufu_console_priv.count)
			cpu_relax();
	}
}

static int __init fufu_console_setup(struct console *c, char *options)
{
	unsigned int baudrate;
	char *n;
	int size;
	vmapi_off_t size_o;

	if (c->index < 0 || c->index >= FUFU_MAX)
	{
		return -ENODEV;
	}

	/* copy first substring: filename */
	n = fufu_bootconsole_filename;
	size = 0;
	while (options && *options && *options != ',' && size < MAX_BOOTCONSOLE_NAME-1)
	{
		*n++ = *options++;
		size++;
	}
	if (!size)
		return -ENODEV;

	fufu_bootconsole_tty = c->index;

	/* hop over the comma */
	if (options && *options)
		options++;

	/* get (optional) baudrate */
	baudrate = 115200;
	if (options) {
		unsigned int number;
		number = simple_strtoul(options, &n, 0);
		if (n != options)
			baudrate = number;
	}

	fufu_bootconsole_handle = vmapi_file_open4(fufu_bootconsole_filename,
						   VMAPI_O_R | VMAPI_O_W,
						   &size_o, NULL);
	if (fufu_bootconsole_handle < 0)
	{
		vmapi_cprintf(FUFU_DEV_NAME ": Failed to open boot console <%s> %d\n",
				fufu_bootconsole_filename, fufu_bootconsole_handle);

		return -ENODEV;
	}
	
	fufu_bootconsole_size = size_o;
	/* in case we can't determine the frame size, fake one */
	if (fufu_bootconsole_size == 0)
		fufu_bootconsole_size = TX_BUF_SIZE;
	fufu_console_priv.buf = kmalloc(fufu_bootconsole_size, GFP_KERNEL);
	if (fufu_console_priv.buf == NULL) {
		vmapi_cprintf(FUFU_DEV_NAME ": Failed to allocate memory for boot console\n");
		vmapi_file_close(fufu_bootconsole_handle);
		fufu_bootconsole_handle = -1;
		return -ENODEV;
	}

	uk_uevent_init(&fufu_console_priv.irq_sleep);
	fufu_console_priv.tx_irq = get_free_async_irq();
	enable_irq_console_thread(fufu_console_priv.tx_irq,
				  "vmfile_tty console",
				  fufu_console_tx_thread,
				  &fufu_console_priv);

	/* wait until thread is sleeping */
	while (uk_uevent_is_waiting(&fufu_console_priv.irq_sleep) == 0)
		cpu_relax();

	return uart_set_options(&fufu_driverinfo[fufu_bootconsole_tty].uart,
				c, baudrate, 'n', 8, 'n');
}

static int __init fufu_console_init(void)
{
	fufu_init_ports();
	register_console(&fufu_console);
	return 0;
}

console_initcall(fufu_console_init);

static struct console fufu_console = {
	.name		= "ttyFP",
	.write		= fufu_console_write,
	.device		= uart_console_device,
	.setup		= fufu_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &fufu_uart,
};

#endif /*CONFIG_P4VM_FILE_CONSOLE*/
