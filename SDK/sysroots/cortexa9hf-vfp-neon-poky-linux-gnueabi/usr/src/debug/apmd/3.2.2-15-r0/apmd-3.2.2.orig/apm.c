/* apm.c -- Simple APM interface
 * Created: Mon Jan  8 10:28:16 1996 by faith@acm.org
 * Revised: Fri Dec 26 20:34:25 1997 by faith@acm.org
 * Copyright 1996, 1997 Rickard E. Faith (faith@acm.org)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>
#include "apm.h"

static int verbose = 0;

/* mode codes for change_state() */
#define SUSPEND 0
#define STANDBY 1
#define IGNORE 2
#define NOIGNORE 3

void change_state(int mode)
{
    int fd;
    time_t then, now;
    int error;

    fd = open(APM_DEVICE, O_WRONLY);
    if (fd < 0)
    {
	fprintf(stderr, "Cannot open APM device: %s\n", strerror(errno));
	exit(1);
    }
    time(&then);
    switch (mode)
    {
    case SUSPEND:
	error = apm_suspend(fd);
	break;
    case STANDBY:
	error = apm_standby(fd);
	break;
#ifdef APM_IOC_IGNORE /* detect kernel support of IGNORE/NOIGNORE functions */
    case IGNORE:
    case NOIGNORE:
	error = apm_set_ignore(fd, mode);
	apm_close(fd);
	return;
	break;
#endif
    default:
	error = 0;
	break;
    }
    
    if (error != 0)
	perror("apm");
    else
	time(&now);

    apm_close(fd);
}

void usage(void)
{
    fprintf(stderr, "usage: apm [-VvmMsSd");
#ifdef APM_IOC_IGNORE		/* detect kernel support of IGNORE/NOIGNORE functions */
    fprintf(stderr, "in");
#endif
    fprintf(stderr,
	    "] [--verbose] [--minutes] [--monitor] [--suspend] [--standby]");
#ifdef APM_IOC_IGNORE		/* detect kernel support of IGNORE/NOIGNORE functions */
    fprintf(stderr, " [--ignore] [--noignore]");
#endif
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{
    apm_info i;
    int suspend = 0;
    int standby = 0;
    int debug = 0;
    int displayMinutes = 0;
	int monitor = 0;
    int c;
    struct option longopts[] =
    {
	{"version", 0, 0, 'V'},
	{"verbose", 0, 0, 'v'},
	{"suspend", 0, 0, 's'},
	{"standby", 0, 0, 'S'},
	{"debug", 0, 0, 'd'},
	{"minutes", 0, 0, 'm'},
	{"monitor", 0, 0, 'M'},
#ifdef APM_IOC_IGNORE		/* detect kernel support of IGNORE/NOIGNORE functions */
	{"ignore", 0, 0, 'i'},
	{"noignore", 0, 0, 'n'},
#endif
	{NULL, 0, 0, 0}
    };
    extern int opterr;

    opterr = 0;
#ifndef APM_IOC_IGNORE		/* detect kernel support of IGNORE/NOIGNORE functions */
    while ((c = getopt_long(argc, argv, "VvsSdmM", longopts, NULL)) != -1)
#else
    while ((c = getopt_long(argc, argv, "VvsSdmMin", longopts, NULL)) != -1)
#endif
	switch (c)
	{
	case 'V':
	    fprintf(stderr, "apm version %s\n", VERSION);
	    exit(0);
	    break;
#ifdef APM_IOC_IGNORE		/* detect kernel support of IGNORE/NOIGNORE functions */
	case 'i':
	    change_state(IGNORE);
	    exit(0);
	    break;
	case 'n':
	    change_state(NOIGNORE);
	    exit(0);
	    break;
#endif
	case 'v':
	    ++verbose;
	    break;
	case 's':
	    ++suspend;
	    break;
	case 'S':
	    ++standby;
	    break;
	case 'd':
	    ++debug;
	    ++verbose;
	    break;
	case 'm':
	    ++displayMinutes;
	    break;
	case 'M':
		++monitor;
		break;
	default:
	    usage();
	    break;
	}

    switch (apm_exists())
    {
    case 1:
	fprintf(stderr, "No APM support in kernel\n");
	exit(1);
    case 2:
	fprintf(stderr, "Old APM support in kernel\n");
	exit(2);
    }


    if (suspend && standby)
	usage();
    if (suspend || standby)
    {
	change_state((suspend ? SUSPEND : STANDBY));
	exit(0);
    }

    if (apm_read(&i))
    {
	fprintf(stderr, "Cannot read APM information\n");
	exit(1);
    }

    if (verbose)
	printf("APM BIOS %d.%d (kernel driver %s)\n",
	     i.apm_version_major, i.apm_version_minor, i.driver_version);

    if (monitor)
	{
		int secs, q = 0;
		char battery[32], output[128] = "", buf[128] = "";
		while(1) {
			apm_read(&i);
			secs = i.using_minutes ? i.battery_time * 60 : i.battery_time;
			if(displayMinutes)
				sprintf(battery, "%d min", secs / 60);
			else
				sprintf(battery, "%s", apm_time(secs));
			sprintf(output, "Line   : %s \nBattery: %d%% (%s)      ",
					i.ac_line_status == 0 ? "off" : "on ",
					i.battery_percentage, battery);
			if(q == 0 || !strcmp(buf, output)) {
				printf("\e[H\e[J%s\n", output);
				q = 1;
			}
			strcpy(buf, output);
			sleep(1);
		}
	}
													    
#if 0
    if (!(i.apm_flags & APM_32_BIT_SUPPORT))
    {
	fprintf(stderr, "32-bit APM interface not supported\n");
	exit(1);
    }
#endif

    if (verbose && (i.apm_flags & 0x10))
	printf("APM BIOS Power Management is currently disabled\n");
    if (verbose && (i.apm_flags & 0x20))
	printf("APM BIOS Power Management is currently disengaged\n");

    switch (i.ac_line_status)
    {
    case 0:
	printf("Off-line");
	break;
    case 1:
	printf("On-line");
	break;
    case 2:
	printf("On backup power");
	break;
    }
    if (i.battery_flags != 0xff	/* Have a 1.1 BIOS and a system battery. */
	&& i.battery_flags & 0x80)
    {
	printf(", no system battery");
    }
    else
    {
	/* Even if battery_flags == 0xff, some 1.0
	 * BIOSs provide this information.  If we
	 * have it, use it. 
	 */
	switch (i.battery_status)
	{
	case 0:
	    printf(", battery status high");
	    break;
	case 1:
	    printf(", battery status low");
	    break;
	case 2:
	    printf(", battery status critical");
	    break;
	case 3:
	    printf(", battery charging");
	    break;
	}
	if (i.battery_percentage >= 0)
	    printf(": %d%%", i.battery_percentage);
	if (i.battery_time >= 0)
	{
	    int secs = i.using_minutes ? i.battery_time * 60 : i.battery_time;
	    if (displayMinutes)
		printf(" (%d min)", secs / 60);
	    else
		printf(" (%s)", apm_time(secs));
	}
    }
    printf("\n");

    if (debug)
    {
	printf("Using device 0x%04lx, %s: %d.%d\n", (unsigned long) apm_dev(),
	     i.driver_version, i.apm_version_major, i.apm_version_minor);
	printf("APM Flags      = 0x%02x; Line Status   = 0x%02x\n",
	       i.apm_flags, i.ac_line_status);
	printf("Battery Status = 0x%02x; Battery Flags = 0x%02x\n",
	       i.battery_status, i.battery_flags);
	printf("Battery %%age   = %4d; Battery Time  = %4d, use_mins=%d\n",
	       i.battery_percentage, i.battery_time, i.using_minutes);
    }

    return 0;
}
