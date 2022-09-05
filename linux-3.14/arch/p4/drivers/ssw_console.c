/*
 *  Early printk on PikeOS system console
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2006-2012 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/console.h>
#include <linux/kernel.h>
#include <asm/debugout.h>

static int early_console_initialized;

static void sswcon_write(struct console *c, const char *s, unsigned count)
{
	unsigned sent;
	unsigned long flags;

	sent = 0;
	while (sent < count) {
		char tmp[128];
		unsigned i;
		/* build single chunk to send, extend \n to \r\n
		 * i is index in tmp
		 */
		i = 0;
		while ((i < sizeof(tmp)-2) && (sent < count)) {
			if (s[sent] == '\n')
			{
				tmp[i] = '\r';
				i++;
			}
			tmp[i] = s[sent];
			i++;
			sent++;
		}

		/* messages might get lost, but we don't care during early boot */
		if (i > 0) {
			local_irq_save(flags);
			tmp[i] = 0;

			vmapi_cprintf("%s", tmp);
			local_irq_restore(flags);
		}
	}
}

static struct console sswcon = {
	name:		"sswcon",
	write:		sswcon_write,
	flags:		CON_BOOT,
	index:		-1,
};

static int __init sswcon_init(char *buf)
{
	if (early_console_initialized)
		return 1;

	register_console(&sswcon);
	early_console_initialized = 1;

	return 0;
}

early_param("earlyprintk", sswcon_init);
