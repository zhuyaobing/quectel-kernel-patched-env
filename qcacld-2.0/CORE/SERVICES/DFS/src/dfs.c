/*
 * Copyright (c) 2002-2014, 2016-2017,2019, 2021 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */

/*===========================================================================

                              dfs.c

  OVERVIEW:

  Source code borrowed from QCA_MAIN DFS module

  DEPENDENCIES:

  Are listed for each API below.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



  when        who     what, where, why
----------    ---    --------------------------------------------------------

===========================================================================*/


#include <osdep.h>
#include "sys/queue.h"

//#include "if_athioctl.h"
//#include "if_athvar.h"
#include "dfs_ioctl.h"
#include "dfs.h"

int domainoverride=DFS_UNINIT_DOMAIN;

/*
 ** channel switch announcement (CSA)
 ** usenol=1 (default) make CSA and switch to a new channel on radar detect
 ** usenol=0, make CSA with next channel same as current on radar detect
 ** usenol=2, no CSA and stay on the same channel on radar detect
 **/

int usenol=1;
u_int32_t dfs_debug_level=ATH_DEBUG_DFS;
#ifdef ATH_SUPPORT_DFS

#if 0 /* the code to call this is curently commented-out below */
/*
 * Mark a channel as having interference detected upon it.
 *
 * This adds the interference marker to both the primary and
 * extension channel.
 *
 * XXX TODO: make the NOL and channel interference logic a bit smarter
 * so only the channel with the radar event is marked, rather than
 * both the primary and extension.
 */
static void
dfs_channel_mark_radar(struct ath_dfs *dfs, struct ieee80211_channel *chan)
{
    struct ieee80211_channel_list chan_info;
    int i;

    //chan->ic_flagext |= CHANNEL_INTERFERENCE;

    /*
     * If radar is detected in 40MHz mode, add both the primary and the
     * extension channels to the NOL. chan is the channel data we return
     * to the ath_dev layer which passes it on to the 80211 layer.
     * As we want the AP to change channels and send out a CSA,
     * we always pass back the primary channel data to the ath_dev layer.
     */
    if ((dfs->dfs_rinfo.rn_use_nol == 1) &&
      (dfs->ic->ic_opmode == IEEE80211_M_HOSTAP ||
      dfs->ic->ic_opmode == IEEE80211_M_IBSS)) {
        chan_info.cl_nchans= 0;
        dfs->ic->ic_get_ext_chan_info (dfs->ic, &chan_info);

        for (i = 0; i < chan_info.cl_nchans; i++)
        {
            if (chan_info.cl_channels[i] == NULL) {
                DFS_PRINTK("%s: NULL channel\n", __func__);
            } else {
                chan_info.cl_channels[i]->ic_flagext |= CHANNEL_INTERFERENCE;
                dfs_nol_addchan(dfs, chan_info.cl_channels[i], dfs->ath_dfs_nol_timeout);
            }
        }


        /*
         * Update the umac/driver channels with the new NOL information.
         */
        dfs_nol_update(dfs);
    }
}
#endif /* #if 0 */

/**
 * dfs_radar_ignore() - ignore radar found event
 * @data: pointer to struct ieee80211com
 *
 * Return: none
 */
static void dfs_radar_ignore(void *data)
{
	struct ieee80211com *ic = (struct ieee80211com *)data;
	struct ath_dfs *dfs = NULL;

	dfs = (struct ath_dfs *)ic->ic_dfs;
	dfs->ath_radar_ignore_after_assoc = false;
}

/**
 * dfs_radar_delay() - delay radar found event
 * @data: pointer to struct ieee80211com
 *
 * Return: none
 */
static void dfs_radar_delay(void *data)
{
	struct ieee80211com *ic = (struct ieee80211com *)data;
	struct ath_dfs *dfs = NULL;

	dfs = (struct ath_dfs *)ic->ic_dfs;

	/*
	 * This calls into the umac DFS code, which sets the umac related
	 * radar flags and begins the channel change machinery.
	 *
	 * XXX TODO: the umac NOL code isn't used, but IEEE80211_CHAN_RADAR
	 * still gets set.  Since the umac NOL code isn't used, that flag
	 * is never cleared.  This needs to be fixed. See EV 105776.
	 */
	if (dfs->dfs_rinfo.rn_use_nol == 1)  {
		ic->ic_dfs_notify_radar(ic, ic->ic_curchan);
	} else if (dfs->dfs_rinfo.rn_use_nol == 0) {
		/*
		 * For the test mode, don't do a CSA here; but setup the
		 * test timer so we get a CSA _back_ to the original channel.
		 */
		OS_CANCEL_TIMER(&dfs->ath_dfstesttimer);
		dfs->ath_dfstest = 1;
		adf_os_spin_lock_bh(&ic->chan_lock);
		dfs->ath_dfstest_ieeechan = ic->ic_curchan->ic_ieee;
		adf_os_spin_unlock_bh(&ic->chan_lock);
		dfs->ath_dfstesttime = 1;   /* 1ms */
		OS_SET_TIMER(&dfs->ath_dfstesttimer, dfs->ath_dfstesttime);
	}
	dfs->ath_radar_delaysched = 0;
}

static OS_TIMER_FUNC(dfs_task)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
	struct ath_dfs *dfs = from_timer(dfs, t, ath_dfs_task_timer);
	struct ieee80211com *ic = dfs->ic;
#else
	struct ieee80211com *ic;
	struct ath_dfs *dfs = NULL;

	OS_GET_TIMER_ARG(ic, struct ieee80211com *);
	dfs = (struct ath_dfs *)ic->ic_dfs;
#endif

	if (dfs_process_radarevent(dfs, ic->ic_curchan)) {
		if (!dfs->dfs_enable_radar_war) {
			/*
			 * This calls into the umac DFS code, which sets the
			 * umac related radar flags and begins the channel
			 * change machinery.
			 *
			 * XXX TODO: the umac NOL code isn't used, but
			 * IEEE80211_CHAN_RADAR still gets set.  Since the umac
			 * NOL code isn't used, that flag is never cleared.
			 * This needs to be fixed. See EV 105776.
			 */
			if (dfs->dfs_rinfo.rn_use_nol == 1)  {
				ic->ic_dfs_notify_radar(ic, ic->ic_curchan);
			} else if (dfs->dfs_rinfo.rn_use_nol == 0) {
				/*
				 * For the test mode, don't do a CSA here; but
				 * setup the test timer so we get a CSA _back_
				 * to the original channel.
				 */
				OS_CANCEL_TIMER(&dfs->ath_dfstesttimer);
				dfs->ath_dfstest = 1;
				adf_os_spin_lock_bh(&ic->chan_lock);
				dfs->ath_dfstest_ieeechan =
					ic->ic_curchan->ic_ieee;
				adf_os_spin_unlock_bh(&ic->chan_lock);
				dfs->ath_dfstesttime = 1;   /* 1ms */
				OS_SET_TIMER(&dfs->ath_dfstesttimer,
					dfs->ath_dfstesttime);
			}
		} else {
			if ((!dfs->ath_radar_delaysched) &&
				(!dfs->ath_radar_ignore_after_assoc)) {
				ic->ic_update_dfs_cac_block_tx(true);
				vos_timer_start(&dfs->ath_dfs_radar_delay_timer,
					DFS_RADAR_DELAY);
				dfs->ath_radar_delaysched = 1;
			}
		}
	}
	dfs->ath_radar_tasksched = 0;
}

static
OS_TIMER_FUNC(dfs_testtimer_task)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
    struct ath_dfs *dfs = from_timer(dfs, t, ath_dfs_task_timer);
    struct ieee80211com *ic = dfs->ic;
#else
    struct ieee80211com *ic = NULL;
    struct ath_dfs *dfs = NULL;

    OS_GET_TIMER_ARG(ic, struct ieee80211com *);
    dfs = (struct ath_dfs *)ic->ic_dfs;
#endif

    /* XXX no locking? */
    dfs->ath_dfstest = 0;

    /*
     * Flip the channel back to the original channel.
     * Make sure this is done properly with a CSA.
     */
    DFS_PRINTK("%s: go back to channel %d\n",
        __func__,
        dfs->ath_dfstest_ieeechan);

    /*
     * XXX The mere existence of this method indirection
     *     to a umac function means this code belongs in
     *     the driver, _not_ here.  Please fix this!
     */
    ic->ic_start_csa(ic, dfs->ath_dfstest_ieeechan);
}


static int dfs_get_debug_info(struct ieee80211com *ic, int type, void *data)
{
   struct ath_dfs                 *dfs=(struct ath_dfs *)ic->ic_dfs;
    if (data) {
        *(u_int32_t *)data = dfs->dfs_proc_phyerr;
    }
    return (int)dfs->dfs_proc_phyerr;
}

/**
 * dfs_alloc_mem_filter() - allocate memory for dfs ft_filters
 * @radarf: pointer holding ft_filters
 *
 * Return: 0-success and 1-failure
*/
static int dfs_alloc_mem_filter(struct dfs_filtertype *radarf)
{
	int status = 0, n, i;

	for (i = 0; i < DFS_MAX_NUM_RADAR_FILTERS; i++) {
		radarf->ft_filters[i] = vos_mem_malloc(
						sizeof(struct dfs_filter));
		if (NULL == radarf->ft_filters[i]) {
			DFS_PRINTK("%s[%d]: mem alloc failed\n",
				    __func__, __LINE__);
			status = 1;
			goto error;
		}
	}

	return status;

error:
	/* free up allocated memory */
	for (n = 0; n < i; n++) {
		if (radarf->ft_filters[n]) {
			vos_mem_free(radarf->ft_filters[n]);
			radarf->ft_filters[i] = NULL;
		}
	}

	DFS_PRINTK("%s[%d]: cannot allocate memory for radar filter types\n",
		    __func__, __LINE__);

	return status;
}

/**
 * dfs_free_filter() - free memory allocated for dfs ft_filters
 * @radarf: pointer holding ft_filters
 *
 * Return: NA
*/
static void dfs_free_filter(struct dfs_filtertype *radarf)
{
	int i;

	for (i = 0; i < DFS_MAX_NUM_RADAR_FILTERS; i++) {
		if (radarf->ft_filters[i]) {
			vos_mem_free(radarf->ft_filters[i]);
			radarf->ft_filters[i] = NULL;
		}
	}
}

int
dfs_attach(struct ieee80211com *ic)
{
    int i, n;
    struct ath_dfs *dfs = (struct ath_dfs *)ic->ic_dfs;
    struct ath_dfs_radar_tab_info radar_info;
#define  N(a)  (sizeof(a)/sizeof(a[0]))

    if (dfs != NULL) {
        /*DFS_DPRINTK(dfs, ATH_DEBUG_DFS1,
          "%s: ic_dfs was not NULL\n",
          __func__);
       */
       return 1;
    }

    dfs = (struct ath_dfs *)OS_MALLOC(NULL, sizeof(struct ath_dfs), GFP_ATOMIC);


    if (dfs == NULL) {
        /*DFS_DPRINTK(dfs, ATH_DEBUG_DFS1,
          "%s: ath_dfs allocation failed\n", __func__);*/
        return 1;
    }

    OS_MEMZERO(dfs, sizeof (struct ath_dfs));

    ic->ic_dfs = (void *)dfs;

    dfs->ic = ic;

    ic->ic_dfs_debug = dfs_get_debug_info;
#ifndef ATH_DFS_RADAR_DETECTION_ONLY
    dfs->dfs_nol = NULL;
#endif

    /*
     * Zero out radar_info.  It's possible that the attach function won't
     * fetch an initial regulatory configuration; you really do want to
     * ensure that the contents indicates there aren't any filters.
     */
    OS_MEMZERO(&radar_info, sizeof(radar_info));
    ic->ic_dfs_attach(ic, &dfs->dfs_caps, &radar_info);
    dfs_clear_stats(ic);
    dfs->dfs_event_log_on = 0;
    OS_INIT_TIMER(NULL, &(dfs->ath_dfs_task_timer), dfs_task, (void *) (ic),
           ADF_DEFERRABLE_TIMER);
    vos_timer_init(&(dfs->ath_dfs_radar_delay_timer), VOS_TIMER_TYPE_SW,
           dfs_radar_delay, (void *) (ic));
    vos_timer_init(&(dfs->ath_dfs_radar_ignore_timer), VOS_TIMER_TYPE_SW,
           dfs_radar_ignore, (void *) (ic));

    dfs->sidx1_sidx2_elems.pl_firstelem = 0;
    dfs->sidx1_sidx2_elems.pl_lastelem = DFS_SIDX1_SIDX2_MASK;
    dfs->sidx1_sidx2_elems.pl_numelems = 0;
#ifndef ATH_DFS_RADAR_DETECTION_ONLY
    OS_INIT_TIMER(NULL, &(dfs->ath_dfstesttimer), dfs_testtimer_task,
        (void *) ic, ADF_DEFERRABLE_TIMER);
    dfs->ath_dfs_cac_time = ATH_DFS_WAIT_MS;
    dfs->ath_dfstesttime = ATH_DFS_TEST_RETURN_PERIOD_MS;
#endif
    ATH_DFSQ_LOCK_INIT(dfs);
    STAILQ_INIT(&dfs->dfs_radarq);
    ATH_ARQ_LOCK_INIT(dfs);
    STAILQ_INIT(&dfs->dfs_arq);
    STAILQ_INIT(&(dfs->dfs_eventq));
    ATH_DFSEVENTQ_LOCK_INIT(dfs);
    dfs->events = (struct dfs_event *)OS_MALLOC(NULL,
                   sizeof(struct dfs_event)*DFS_MAX_EVENTS,
                   GFP_ATOMIC);
    if (dfs->events == NULL) {
        OS_FREE(dfs);
        ic->ic_dfs = NULL;
        DFS_PRINTK("%s: events allocation failed\n", __func__);
        return 1;
    }
    for (i = 0; i < DFS_MAX_EVENTS; i++) {
        STAILQ_INSERT_TAIL(&(dfs->dfs_eventq), &dfs->events[i], re_list);
    }

    dfs->pulses = (struct dfs_pulseline *)OS_MALLOC(NULL, sizeof(struct dfs_pulseline), GFP_ATOMIC);
    if (dfs->pulses == NULL) {
            OS_FREE(dfs->events);
            dfs->events = NULL;
            OS_FREE(dfs);
            ic->ic_dfs = NULL;
            DFS_PRINTK("%s: pulse buffer allocation failed\n", __func__);
            return 1;
    }

    dfs->pulses->pl_lastelem = DFS_MAX_PULSE_BUFFER_MASK;

    /* Allocate memory for radar filters */
    for (n = 0; n < DFS_MAX_RADAR_TYPES; n++) {
      dfs->dfs_radarf[n] = (struct dfs_filtertype *)OS_MALLOC(NULL, sizeof(struct dfs_filtertype),GFP_ATOMIC);
      if (dfs->dfs_radarf[n] == NULL) {
         DFS_PRINTK("%s: cannot allocate memory for radar filter types\n",
            __func__);
         goto bad1;
      } else {
        vos_mem_zero(dfs->dfs_radarf[n], sizeof(struct dfs_filtertype));
        if (0 != dfs_alloc_mem_filter(dfs->dfs_radarf[n]))
            goto bad1;
      }
    }

    /* Allocate memory for dc radar filters */
    for (n = 0; n < DFS_MAX_RADAR_TYPES; n++) {
      dfs->dfs_dc_radarf[n] =
          (struct dfs_filtertype *)OS_MALLOC(NULL,
                  sizeof(struct dfs_filtertype), GFP_ATOMIC);
      if (!(dfs->dfs_dc_radarf[n])) {
         DFS_PRINTK("%s: cannot allocate memory for dc radar filter types\n",
            __func__);
         goto bad4;
      }
      vos_mem_zero(dfs->dfs_dc_radarf[n], sizeof(struct dfs_filtertype));
      if (0 != dfs_alloc_mem_filter(dfs->dfs_dc_radarf[n]))
          goto bad4;
    }

    /* Allocate memory for radar table */
    dfs->dfs_radartable = (int8_t **)OS_MALLOC(NULL,
        MAX_DFS_RADAR_TABLE_TYPE * sizeof(int8_t *), GFP_ATOMIC);
    if (dfs->dfs_radartable == NULL) {
      DFS_PRINTK("%s: cannot allocate memory for radar table\n",
         __func__);
      goto bad4;
    }
    for (n = 0; n < MAX_DFS_RADAR_TABLE_TYPE; n++) {
      dfs->dfs_radartable[n] = OS_MALLOC(NULL, DFS_MAX_RADAR_OVERLAP * sizeof(int8_t),
                   GFP_ATOMIC);
      if (dfs->dfs_radartable[n] == NULL) {
         DFS_PRINTK("%s: cannot allocate memory for radar table entry\n",
            __func__);
         goto bad2;
      }
    }

    /* Allocate memory for dc radar table */
    dfs->dfs_dc_radartable = (int8_t **)OS_MALLOC(NULL,
                                 MAX_DFS_RADAR_TABLE_TYPE * sizeof(int8_t *),
                                 GFP_ATOMIC);
    if (!dfs->dfs_dc_radartable) {
      DFS_PRINTK("%s: cannot allocate memory for radar table\n",
         __func__);
      goto bad2;
    }
    for (n = 0; n < MAX_DFS_RADAR_TABLE_TYPE; n++) {
      dfs->dfs_dc_radartable[n] = OS_MALLOC(NULL,
                                      DFS_MAX_RADAR_OVERLAP * sizeof(int8_t),
                                      GFP_ATOMIC);
      if (!(dfs->dfs_dc_radartable[n])) {
         DFS_PRINTK("%s: cannot allocate memory for dc radar table entry\n",
            __func__);
         goto bad3;
      }
    }

    if (usenol == 0)
        DFS_PRINTK("%s: NOL disabled\n", __func__);
    else if (usenol == 2)
        DFS_PRINTK("%s: NOL disabled; no CSA\n", __func__);

    dfs->dfs_rinfo.rn_use_nol = usenol;

    /* Init the cached extension channel busy for false alarm reduction */
    dfs->dfs_rinfo.ext_chan_busy_ts = ic->ic_get_TSF64(ic);
    dfs->dfs_rinfo.dfs_ext_chan_busy = 0;
    /* Init the Bin5 chirping related data */
    dfs->dfs_rinfo.dfs_bin5_chirp_ts = dfs->dfs_rinfo.ext_chan_busy_ts;
    dfs->dfs_rinfo.dfs_last_bin5_dur = MAX_BIN5_DUR;
    dfs->dfs_b5radars = NULL;

    /*
     * If dfs_init_radar_filters() fails, we can abort here and
     * reconfigure when the first valid channel + radar config
     * is available.
     */
    if ( dfs_init_radar_filters( ic,  &radar_info) ) {
        DFS_PRINTK(" %s: Radar Filter Intialization Failed \n",
                    __func__);
            return 1;
    }

    dfs->ath_dfs_false_rssi_thres = RSSI_POSSIBLY_FALSE;
    dfs->ath_dfs_peak_mag = SEARCH_FFT_REPORT_PEAK_MAG_THRSH;
    dfs->dfs_phyerr_freq_min     = 0x7fffffff;
    dfs->dfs_phyerr_freq_max     = 0;
    dfs->dfs_phyerr_queued_count = 0;
    dfs->dfs_phyerr_w53_counter  = 0;
    dfs->dfs_pri_multiplier      = 2;

    dfs->ath_dfs_nol_timeout = DFS_NOL_TIMEOUT_S;
    return 0;

bad3:
    for (n = 0; n < MAX_DFS_RADAR_TABLE_TYPE; n++) {
       if (dfs->dfs_dc_radartable[n]) {
          OS_FREE(dfs->dfs_dc_radartable[n]);
          dfs->dfs_dc_radartable[n] = NULL;
       }
    }
    OS_FREE(dfs->dfs_dc_radartable);
    dfs->dfs_dc_radartable = NULL;
bad2:
    for (n=0; n < MAX_DFS_RADAR_TABLE_TYPE; n++) {
       if (dfs->dfs_radartable[n] != NULL) {
          OS_FREE(dfs->dfs_radartable[n]);
          dfs->dfs_radartable[n] = NULL;
       }
    }
    OS_FREE(dfs->dfs_radartable);
    dfs->dfs_radartable = NULL;
bad4:
    for (n = 0; n < DFS_MAX_RADAR_TYPES; n++) {
        if (dfs->dfs_dc_radarf[n]) {
         dfs_free_filter(dfs->dfs_dc_radarf[n]);
         OS_FREE(dfs->dfs_dc_radarf[n]);
         dfs->dfs_dc_radarf[n] = NULL;
        }
    }
bad1:
    for (n=0; n<DFS_MAX_RADAR_TYPES; n++) {
        if (dfs->dfs_radarf[n] != NULL) {
         dfs_free_filter(dfs->dfs_radarf[n]);
         OS_FREE(dfs->dfs_radarf[n]);
         dfs->dfs_radarf[n] = NULL;
        }
    }
    if (dfs->pulses) {
        OS_FREE(dfs->pulses);
        dfs->pulses = NULL;
    }
    if (dfs->events) {
        OS_FREE(dfs->events);
        dfs->events = NULL;
    }

    if (ic->ic_dfs) {
        OS_FREE(ic->ic_dfs);
        ic->ic_dfs = NULL;
    }
    return 1;
#undef N
}

void
dfs_detach(struct ieee80211com *ic)
{
   struct ath_dfs *dfs = (struct ath_dfs *)ic->ic_dfs;
   int n, empty;

   if (dfs == NULL) {
           DFS_DPRINTK(dfs, ATH_DEBUG_DFS1, "%s: ic_dfs is NULL\n", __func__);
      return;
   }

        /* Bug 29099 make sure all outstanding timers are cancelled*/

        if (dfs->ath_radar_tasksched) {
            OS_CANCEL_TIMER(&dfs->ath_dfs_task_timer);
            dfs->ath_radar_tasksched = 0;
        }

   vos_timer_stop(&dfs->ath_dfs_radar_delay_timer);
   vos_timer_destroy(&dfs->ath_dfs_radar_delay_timer);

   vos_timer_stop(&dfs->ath_dfs_radar_ignore_timer);
   vos_timer_destroy(&dfs->ath_dfs_radar_ignore_timer);
   if (dfs->ath_dfstest) {
      OS_CANCEL_TIMER(&dfs->ath_dfstesttimer);
      dfs->ath_dfstest = 0;
   }

#if 0
#ifndef ATH_DFS_RADAR_DETECTION_ONLY
        if (dfs->ic_dfswait) {
            OS_CANCEL_TIMER(&dfs->ic_dfswaittimer);
            dfs->ath_dfswait = 0;
        }

      OS_CANCEL_TIMER(&dfs->sc_dfs_war_timer);
   if (dfs->dfs_nol != NULL) {
       struct dfs_nolelem *nol, *next;
       nol = dfs->dfs_nol;
                /* Bug 29099 - each NOL element has its own timer, cancel it and
                   free the element*/
      while (nol != NULL) {
                       OS_CANCEL_TIMER(&nol->nol_timer);
             next = nol->nol_next;
             OS_FREE(nol);
             nol = next;
      }
      dfs->dfs_nol = NULL;
   }
#endif
#endif
        /* Return radar events to free q*/
        dfs_reset_radarq(dfs);
   dfs_reset_alldelaylines(dfs);

        /* Free up pulse log*/
        if (dfs->pulses != NULL) {
                OS_FREE(dfs->pulses);
                dfs->pulses = NULL;
        }

   for (n=0; n<DFS_MAX_RADAR_TYPES;n++) {
      if (dfs->dfs_radarf[n] != NULL) {
         dfs_free_filter(dfs->dfs_radarf[n]);
         OS_FREE(dfs->dfs_radarf[n]);
         dfs->dfs_radarf[n] = NULL;
      }
   }
   for (n = 0; n < DFS_MAX_RADAR_TYPES; n++) {
      if (dfs->dfs_dc_radarf[n]) {
         dfs_free_filter(dfs->dfs_dc_radarf[n]);
         OS_FREE(dfs->dfs_dc_radarf[n]);
         dfs->dfs_dc_radarf[n] = NULL;
      }
   }



   if (dfs->dfs_radartable != NULL) {
      for (n = 0; n < MAX_DFS_RADAR_TABLE_TYPE; n++) {
         if (dfs->dfs_radartable[n] != NULL) {
            OS_FREE(dfs->dfs_radartable[n]);
            dfs->dfs_radartable[n] = NULL;
         }
      }
      OS_FREE(dfs->dfs_radartable);
      dfs->dfs_radartable = NULL;
#ifndef ATH_DFS_RADAR_DETECTION_ONLY
      dfs->ath_dfs_isdfsregdomain = 0;
#endif
   }

   if (dfs->dfs_dc_radartable) {
      for (n = 0; n < MAX_DFS_RADAR_TABLE_TYPE; n++) {
         if (dfs->dfs_dc_radartable[n]) {
            OS_FREE(dfs->dfs_dc_radartable[n]);
            dfs->dfs_dc_radartable[n] = NULL;
         }
      }
      OS_FREE(dfs->dfs_dc_radartable);
      dfs->dfs_dc_radartable = NULL;
   }

   if (dfs->dfs_b5radars != NULL) {
      OS_FREE(dfs->dfs_b5radars);
      dfs->dfs_b5radars=NULL;
   }

/*      Commenting out since all the ar functions are obsolete and
 *      the function definition has been removed as part of dfs_ar.c
 * dfs_reset_ar(dfs);
 */
   ATH_ARQ_LOCK(dfs);
   empty = STAILQ_EMPTY(&(dfs->dfs_arq));
   ATH_ARQ_UNLOCK(dfs);
   if (!empty) {
/*
 *      Commenting out since all the ar functions are obsolete and
 *      the function definition has been removed as part of dfs_ar.c
 *
 *    dfs_reset_arq(dfs);
 */
   }
        if (dfs->events != NULL) {
                OS_FREE(dfs->events);
                dfs->events = NULL;
        }
       dfs_nol_timer_cleanup(dfs);
   OS_FREE(dfs);

   /* XXX? */
        ic->ic_dfs = NULL;
}
/*
 * This is called each time a channel change occurs, to (potentially) enable
 * the radar code.
 */
int dfs_radar_disable(struct ieee80211com *ic)
{
    struct ath_dfs                 *dfs=(struct ath_dfs *)ic->ic_dfs;
#ifdef ATH_ENABLE_AR
    dfs->dfs_proc_phyerr &= ~DFS_AR_EN;
#endif
    dfs->dfs_proc_phyerr &= ~DFS_RADAR_EN;
    return 0;
}
/*
 * This is called each time a channel change occurs, to (potentially) enable
 * the radar code.
 */
int dfs_radar_enable(struct ieee80211com *ic,
    struct ath_dfs_radar_tab_info *radar_info)
{
    int                                 is_ext_ch;
    int                                 is_fastclk = 0;
    int                                 radar_filters_init_status = 0;
    //u_int32_t                        rfilt;
    struct ath_dfs                      *dfs;
    struct dfs_state *rs_pri, *rs_ext;
    struct ieee80211_channel *chan=ic->ic_curchan, *ext_ch = NULL;
    is_ext_ch=IEEE80211_IS_CHAN_11N_HT40(ic->ic_curchan);
    dfs=(struct ath_dfs *)ic->ic_dfs;
    rs_pri = NULL;
    rs_ext = NULL;
#if 0
    int i;
#endif
   if (dfs == NULL) {
                DFS_DPRINTK(dfs, ATH_DEBUG_DFS, "%s: ic_dfs is NULL\n",
         __func__);

                return -EIO;
   }
           ic->ic_dfs_disable(ic);

   /*
    * Setting country code might change the DFS domain
    * so initialize the DFS Radar filters
    */
   radar_filters_init_status = dfs_init_radar_filters(ic, radar_info);

   /*
    * dfs_init_radar_filters() returns 1 on failure and
    * 0 on success.
    */
   if ( DFS_STATUS_FAIL == radar_filters_init_status ) {
      VOS_TRACE(VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                      "%s[%d]: DFS Radar Filters Initialization Failed",
                       __func__,  __LINE__);
      return -EIO;
   }

   if ((ic->ic_opmode == IEEE80211_M_HOSTAP || ic->ic_opmode == IEEE80211_M_IBSS)) {

                if (IEEE80211_IS_CHAN_DFS(chan)) {

         u_int8_t index_pri, index_ext;
#ifdef ATH_ENABLE_AR
    dfs->dfs_proc_phyerr |= DFS_AR_EN;
#endif
    dfs->dfs_proc_phyerr |= DFS_RADAR_EN;



                   if (is_ext_ch) {
                       ext_ch = ieee80211_get_extchan(ic);
                   }
         dfs_reset_alldelaylines(dfs);

         rs_pri = dfs_getchanstate(dfs, &index_pri, 0);
         if (ext_ch) {
                            rs_ext = dfs_getchanstate(dfs, &index_ext, 1);
                   }
         if (rs_pri != NULL && ((ext_ch==NULL)||(rs_ext != NULL))) {
             struct ath_dfs_phyerr_param pe;

             OS_MEMSET(&pe, '\0', sizeof(pe));

             if (index_pri != dfs->dfs_curchan_radindex)
                 dfs_reset_alldelaylines(dfs);

             dfs->dfs_curchan_radindex = (int16_t) index_pri;
             dfs->dfs_pri_multiplier_ini = radar_info->dfs_pri_multiplier;

             if (rs_ext)
                 dfs->dfs_extchan_radindex = (int16_t) index_ext;

             ath_dfs_phyerr_param_copy(&pe,
                     &rs_pri->rs_param);
             DFS_DPRINTK(dfs, ATH_DEBUG_DFS3,
                     "%s: firpwr=%d, rssi=%d, height=%d, "
                     "prssi=%d, inband=%d, relpwr=%d, "
                     "relstep=%d, maxlen=%d\n",
                     __func__,
                     pe.pe_firpwr,
                     pe.pe_rrssi,
                     pe.pe_height,
                     pe.pe_prssi,
                     pe.pe_inband,
                     pe.pe_relpwr,
                     pe.pe_relstep,
                     pe.pe_maxlen
                     );

             ic->ic_dfs_enable(ic, &is_fastclk, &pe);
             DFS_DPRINTK(dfs, ATH_DEBUG_DFS, "Enabled radar detection on channel %d\n",
                     chan->ic_freq);
             dfs->dur_multiplier =
                 is_fastclk ? DFS_FAST_CLOCK_MULTIPLIER : DFS_NO_FAST_CLOCK_MULTIPLIER;
             DFS_DPRINTK(dfs, ATH_DEBUG_DFS3,
                     "%s: duration multiplier is %d\n", __func__, dfs->dur_multiplier);
         } else
             DFS_DPRINTK(dfs, ATH_DEBUG_DFS, "%s: No more radar states left\n",
                     __func__);
      }
   }

   return DFS_STATUS_SUCCESS;
}

int
dfs_control(struct ieee80211com *ic, u_int id,
                void *indata, u_int32_t insize,
                void *outdata, u_int32_t *outsize)
{
   int error = 0;
   struct ath_dfs_phyerr_param peout;
   struct ath_dfs *dfs = (struct ath_dfs *)ic->ic_dfs;
   struct dfs_ioctl_params *dfsparams;
    u_int32_t val=0;
#ifndef ATH_DFS_RADAR_DETECTION_ONLY
    struct dfsreq_nolinfo *nol;
    u_int32_t *data = NULL;
#endif /* ATH_DFS_RADAR_DETECTION_ONLY */
    int i;

   if (dfs == NULL) {
      error = -EINVAL;
                DFS_DPRINTK(dfs, ATH_DEBUG_DFS1, "%s DFS is null\n", __func__);
      goto bad;
   }


   switch (id) {
   case DFS_SET_THRESH:
      if (insize < sizeof(struct dfs_ioctl_params) || !indata) {
         DFS_DPRINTK(dfs, ATH_DEBUG_DFS1,
             "%s: insize=%d, expected=%zu bytes, indata=%pK\n",
             __func__, insize, sizeof(struct dfs_ioctl_params),
             indata);
         error = -EINVAL;
         break;
      }
      dfsparams = (struct dfs_ioctl_params *) indata;
      if (!dfs_set_thresholds(ic, DFS_PARAM_FIRPWR, dfsparams->dfs_firpwr))
         error = -EINVAL;
      if (!dfs_set_thresholds(ic, DFS_PARAM_RRSSI, dfsparams->dfs_rrssi))
         error = -EINVAL;
      if (!dfs_set_thresholds(ic, DFS_PARAM_HEIGHT, dfsparams->dfs_height))
         error = -EINVAL;
      if (!dfs_set_thresholds(ic, DFS_PARAM_PRSSI, dfsparams->dfs_prssi))
         error = -EINVAL;
      if (!dfs_set_thresholds(ic, DFS_PARAM_INBAND, dfsparams->dfs_inband))
         error = -EINVAL;
      /* 5413 speicfic */
      if (!dfs_set_thresholds(ic, DFS_PARAM_RELPWR, dfsparams->dfs_relpwr))
         error = -EINVAL;
      if (!dfs_set_thresholds(ic, DFS_PARAM_RELSTEP, dfsparams->dfs_relstep))
         error = -EINVAL;
      if (!dfs_set_thresholds(ic, DFS_PARAM_MAXLEN, dfsparams->dfs_maxlen))
         error = -EINVAL;
      break;
   case DFS_GET_THRESH:
      if (!outdata || !outsize || *outsize <sizeof(struct dfs_ioctl_params)) {
         error = -EINVAL;
         break;
      }
      *outsize = sizeof(struct dfs_ioctl_params);
      dfsparams = (struct dfs_ioctl_params *) outdata;

      /*
       * Fetch the DFS thresholds using the internal representation.
       */
      (void) dfs_get_thresholds(ic, &peout);

      /*
       * Convert them to the dfs IOCTL representation.
       */
      ath_dfs_dfsparam_to_ioctlparam(&peout, dfsparams);
                break;
   case DFS_RADARDETECTS:
      if (!outdata || !outsize || *outsize < sizeof(u_int32_t)) {
         error = -EINVAL;
         break;
      }
      *outsize = sizeof (u_int32_t);
      *((u_int32_t *)outdata) = dfs->ath_dfs_stats.num_radar_detects;
      break;
        case DFS_DISABLE_DETECT:
                dfs->dfs_proc_phyerr &= ~DFS_RADAR_EN;
                dfs->ic->ic_dfs_state.ignore_dfs = 1;
                DFS_PRINTK("%s enable detects, ignore_dfs %d\n",
                    __func__,
          dfs->ic->ic_dfs_state.ignore_dfs);
                break;
        case DFS_ENABLE_DETECT:
      dfs->dfs_proc_phyerr |= DFS_RADAR_EN;
                dfs->ic->ic_dfs_state.ignore_dfs = 0;
                DFS_PRINTK("%s enable detects, ignore_dfs %d\n",
                    __func__,
          dfs->ic->ic_dfs_state.ignore_dfs);
                break;
        case DFS_DISABLE_FFT:
                //UMACDFS: TODO: val = ath_hal_dfs_config_fft(sc->sc_ah, false);
                DFS_PRINTK("%s TODO disable FFT val=0x%x \n", __func__, val);
                break;
        case DFS_ENABLE_FFT:
                //UMACDFS TODO: val = ath_hal_dfs_config_fft(sc->sc_ah, true);
                DFS_PRINTK("%s TODO enable FFT val=0x%x \n", __func__, val);
                break;
        case DFS_SET_DEBUG_LEVEL:
      if (insize < sizeof(u_int32_t) || !indata) {
         error = -EINVAL;
         break;
      }
      dfs->dfs_debug_mask= *(u_int32_t *)indata;
      DFS_PRINTK("%s debug level now = 0x%x \n",
          __func__,
          dfs->dfs_debug_mask);
                if (dfs->dfs_debug_mask & ATH_DEBUG_DFS3) {
                    /* Enable debug Radar Event */
                    dfs->dfs_event_log_on = 1;
                } else {
                    dfs->dfs_event_log_on = 0;
                }
                break;
        case DFS_SET_FALSE_RSSI_THRES:
      if (insize < sizeof(u_int32_t) || !indata) {
         error = -EINVAL;
         break;
      }
      dfs->ath_dfs_false_rssi_thres= *(u_int32_t *)indata;
      DFS_PRINTK("%s false RSSI threshold now = 0x%x \n",
          __func__,
          dfs->ath_dfs_false_rssi_thres);
                break;
        case DFS_SET_PEAK_MAG:
      if (insize < sizeof(u_int32_t) || !indata) {
         error = -EINVAL;
         break;
      }
      dfs->ath_dfs_peak_mag= *(u_int32_t *)indata;
      DFS_PRINTK("%s peak_mag now = 0x%x \n",
          __func__,
          dfs->ath_dfs_peak_mag);
                break;
        case DFS_IGNORE_CAC:
      if (insize < sizeof(u_int32_t) || !indata) {
         error = -EINVAL;
         break;
      }
             if (*(u_int32_t *)indata) {
                 dfs->ic->ic_dfs_state.ignore_cac= 1;
             } else {
                 dfs->ic->ic_dfs_state.ignore_cac= 0;
             }
      DFS_PRINTK("%s ignore cac = 0x%x \n",
          __func__,
          dfs->ic->ic_dfs_state.ignore_cac);
                break;
        case DFS_SET_NOL_TIMEOUT:
      if (insize < sizeof(u_int32_t) || !indata) {
         error = -EINVAL;
         break;
      }
             if (*(int *)indata) {
                 dfs->ath_dfs_nol_timeout= *(int *)indata;
             } else {
                 dfs->ath_dfs_nol_timeout= DFS_NOL_TIMEOUT_S;
             }
      DFS_PRINTK("%s nol timeout = %d sec \n",
          __func__,
          dfs->ath_dfs_nol_timeout);
                break;
#ifndef ATH_DFS_RADAR_DETECTION_ONLY
    case DFS_MUTE_TIME:
        if (insize < sizeof(u_int32_t) || !indata) {
            error = -EINVAL;
            break;
        }
        data = (u_int32_t *) indata;
        dfs->ath_dfstesttime = *data;
        dfs->ath_dfstesttime *= (1000); //convert sec into ms
        break;
   case DFS_GET_USENOL:
      if (!outdata || !outsize || *outsize < sizeof(u_int32_t)) {
         error = -EINVAL;
         break;
      }
      *outsize = sizeof(u_int32_t);
      *((u_int32_t *)outdata) = dfs->dfs_rinfo.rn_use_nol;



                 for (i = 0; (i < DFS_EVENT_LOG_SIZE) && (i < dfs->dfs_event_log_count); i++) {
                         //DFS_DPRINTK(sc, ATH_DEBUG_DFS,"ts=%llu diff_ts=%u rssi=%u dur=%u\n", dfs->radar_log[i].ts, dfs->radar_log[i].diff_ts, dfs->radar_log[i].rssi, dfs->radar_log[i].dur);

                 }
                 dfs->dfs_event_log_count     = 0;
                 dfs->dfs_phyerr_count        = 0;
                 dfs->dfs_phyerr_reject_count = 0;
                 dfs->dfs_phyerr_queued_count = 0;
                dfs->dfs_phyerr_freq_min     = 0x7fffffff;
                dfs->dfs_phyerr_freq_max     = 0;
      break;
   case DFS_SET_USENOL:
      if (insize < sizeof(u_int32_t) || !indata) {
         error = -EINVAL;
         break;
      }
      dfs->dfs_rinfo.rn_use_nol = *(u_int32_t *)indata;
      /* iwpriv markdfs in linux can do the same thing... */
      break;
   case DFS_GET_NOL:
      if (!outdata || !outsize || *outsize < sizeof(struct dfsreq_nolinfo)) {
         error = -EINVAL;
         break;
      }
      *outsize = sizeof(struct dfsreq_nolinfo);
      nol = (struct dfsreq_nolinfo *)outdata;
      dfs_get_nol(dfs, (struct dfsreq_nolelem *)nol->dfs_nol, &nol->ic_nchans);
             dfs_print_nol(dfs);
      break;
   case DFS_SET_NOL:
      if (insize < sizeof(struct dfsreq_nolinfo) || !indata) {
                        error = -EINVAL;
                        break;
                }
                nol = (struct dfsreq_nolinfo *) indata;
      dfs_set_nol(dfs, (struct dfsreq_nolelem *)nol->dfs_nol, nol->ic_nchans);
      break;

    case DFS_SHOW_NOL:
        dfs_print_nol(dfs);
        break;
    case DFS_BANGRADAR:
 #if 0 //MERGE_TBD
        if(sc->sc_nostabeacons)
        {
            printk("No radar detection Enabled \n");
            break;
        }
#endif
        dfs->dfs_bangradar = 1;
        dfs->ath_radar_tasksched = 1;
        OS_SET_TIMER(&dfs->ath_dfs_task_timer, 0);
        break;
#endif /* ATH_DFS_RADAR_DETECTION_ONLY */
   default:
      error = -EINVAL;
   }
bad:
   return error;
}
int
dfs_set_thresholds(struct ieee80211com *ic, const u_int32_t threshtype,
         const u_int32_t value)
{
   struct ath_dfs *dfs = (struct ath_dfs *)ic->ic_dfs;
   int16_t chanindex;
   struct dfs_state *rs;
   struct ath_dfs_phyerr_param pe;
   int is_fastclk = 0;  /* XXX throw-away */

   if (dfs == NULL) {
      DFS_DPRINTK(dfs, ATH_DEBUG_DFS1, "%s: ic_dfs is NULL\n",
         __func__);
      return 0;
   }

   chanindex = dfs->dfs_curchan_radindex;
   if ((chanindex <0) || (chanindex >= DFS_NUM_RADAR_STATES)) {
      DFS_DPRINTK(dfs, ATH_DEBUG_DFS1,
          "%s: chanindex = %d, DFS_NUM_RADAR_STATES=%d\n",
          __func__,
          chanindex,
          DFS_NUM_RADAR_STATES);
      return 0;
   }

   DFS_DPRINTK(dfs, ATH_DEBUG_DFS,
       "%s: threshtype=%d, value=%d\n", __func__, threshtype, value);

   ath_dfs_phyerr_init_noval(&pe);

   rs = &(dfs->dfs_radar[chanindex]);
   switch (threshtype) {
   case DFS_PARAM_FIRPWR:
      rs->rs_param.pe_firpwr = (int32_t) value;
      pe.pe_firpwr = value;
      break;
   case DFS_PARAM_RRSSI:
      rs->rs_param.pe_rrssi = value;
      pe.pe_rrssi = value;
      break;
   case DFS_PARAM_HEIGHT:
      rs->rs_param.pe_height = value;
      pe.pe_height = value;
      break;
   case DFS_PARAM_PRSSI:
      rs->rs_param.pe_prssi = value;
      pe.pe_prssi = value;
      break;
   case DFS_PARAM_INBAND:
      rs->rs_param.pe_inband = value;
      pe.pe_inband = value;
      break;
   /* 5413 specific */
   case DFS_PARAM_RELPWR:
      rs->rs_param.pe_relpwr = value;
      pe.pe_relpwr = value;
      break;
   case DFS_PARAM_RELSTEP:
      rs->rs_param.pe_relstep = value;
      pe.pe_relstep = value;
      break;
   case DFS_PARAM_MAXLEN:
      rs->rs_param.pe_maxlen = value;
      pe.pe_maxlen = value;
      break;
   default:
      DFS_DPRINTK(dfs, ATH_DEBUG_DFS1,
          "%s: unknown threshtype (%d)\n",
          __func__,
          threshtype);
      break;
   }

   /*
    * The driver layer dfs_enable routine is tasked with translating
    * values from the global format to the per-device (HAL, offload)
    * format.
    */
   ic->ic_dfs_enable(ic, &is_fastclk, &pe);
   return 1;
}

int
dfs_get_thresholds(struct ieee80211com *ic, struct ath_dfs_phyerr_param *param)
{
   //UMACDFS : TODO:ath_hal_getdfsthresh(sc->sc_ah, param);

   OS_MEMZERO(param, sizeof(*param));

   (void) ic->ic_dfs_get_thresholds(ic, param);

   return 1;
}

u_int16_t   dfs_usenol(struct ieee80211com *ic)
{
    struct ath_dfs *dfs = (struct ath_dfs *)ic->ic_dfs;
    return dfs ? (u_int16_t) dfs->dfs_rinfo.rn_use_nol : 0;
}

u_int16_t   dfs_isdfsregdomain(struct ieee80211com *ic)
{
    struct ath_dfs *dfs = (struct ath_dfs *)ic->ic_dfs;
    return dfs ? dfs->dfsdomain : 0;
}
#else
int
dfs_attach(struct ieee80211com *ic)
{
	return 0;
}
void
dfs_detach(struct ieee80211com *ic)
{
}
#endif /* ATH_UPPORT_DFS */
