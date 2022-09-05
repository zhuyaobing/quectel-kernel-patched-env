/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
 */

#include <stdio.h>
#include <stdint.h>
#include <alsa/asoundlib.h>
#include <alsa/pcm_rate.h>

#include "asrc_pair.h"

struct rate_src {
	int type;
	unsigned int channels;
    asrc_pair *pair;
};

static snd_pcm_uframes_t input_frames(void *obj, snd_pcm_uframes_t frames)
{
   uint32_t num, den;
   struct rate_src *rate = obj;
   if (frames == 0)
      return 0;
   asrc_pair_get_ratio(rate->pair, &num, &den);
   return (snd_pcm_uframes_t)((frames * num + (den >> 1)) / den);
}

static snd_pcm_uframes_t output_frames(void *obj, snd_pcm_uframes_t frames)
{
   uint32_t num, den;
   struct rate_src *rate = obj;
   if (frames == 0)
      return 0;
   asrc_pair_get_ratio(rate->pair, &num, &den);
   return (snd_pcm_uframes_t)((frames * den + (num >> 1)) / num);
}

static void pcm_src_free(void *obj)
{
   struct rate_src *rate = obj;
   if (rate->pair)
   {
      asrc_pair_destroy(rate->pair);
      rate->pair = NULL;
   }
}

static int pcm_src_init(void *obj, snd_pcm_rate_info_t *info)
{
   struct rate_src *rate = obj;
   
   if (!rate->pair || rate->channels != info->channels)
   {
      if (rate->pair)
         asrc_pair_destroy(rate->pair);
      rate->channels = info->channels;
      rate->pair = asrc_pair_create(rate->channels, info->in.period_size * rate->channels,
              info->out.period_size * rate->channels, info->in.rate, info->out.rate, rate->type);
      if (!rate->pair)
         return -EINVAL;
   }

   return 0;
}

static int pcm_src_adjust_pitch(void *obj, snd_pcm_rate_info_t *info)
{
   struct rate_src *rate = obj;
   return asrc_pair_set_rate(rate->pair, info->in.period_size * rate->channels,
           info->out.period_size * rate->channels, info->in.rate, info->out.rate);
}

static void pcm_src_reset(void *obj)
{
   struct rate_src *rate = obj;
   asrc_pair_reset(rate->pair);
}

static void pcm_src_convert_s16(void *obj, int16_t *dst, unsigned int dst_frames,
				const int16_t *src, unsigned int src_frames)
{
   struct rate_src *rate = obj;
   asrc_pair_convert_s16(rate->pair, src, src_frames * rate->channels, dst, dst_frames * rate->channels);
}

static void pcm_src_close(void *obj)
{
   free(obj);
}

#if SND_PCM_RATE_PLUGIN_VERSION >= 0x010002
static int get_supported_rates(void *obj, unsigned int *rate_min,
			       unsigned int *rate_max)
{
	*rate_min = 8000;
    *rate_max = 192000;
	return 0;
}

static void dump(void *obj, snd_output_t *out)
{
	snd_output_printf(out, "Converter: asrc\n");
}
#endif

static snd_pcm_rate_ops_t pcm_src_ops = {
	.close = pcm_src_close,
	.init = pcm_src_init,
	.free = pcm_src_free,
	.reset = pcm_src_reset,
	.adjust_pitch = pcm_src_adjust_pitch,
	.convert_s16 = pcm_src_convert_s16,
	.input_frames = input_frames,
	.output_frames = output_frames,
#if SND_PCM_RATE_PLUGIN_VERSION >= 0x010002
	.version = SND_PCM_RATE_PLUGIN_VERSION,
	.get_supported_rates = get_supported_rates,
	.dump = dump,
#endif
};

static int pcm_src_open(unsigned int version, void **objp,
			snd_pcm_rate_ops_t *ops, int type)
{
	struct rate_src *rate;

#if SND_PCM_RATE_PLUGIN_VERSION < 0x010002
	if (version != SND_PCM_RATE_PLUGIN_VERSION) {
		fprintf(stderr, "Invalid rate plugin version %x\n", version);
		return -EINVAL;
	}
#endif
	rate = calloc(1, sizeof(*rate));
	if (!rate)
		return -ENOMEM;
	rate->type = type;

	*objp = rate;
#if SND_PCM_RATE_PLUGIN_VERSION >= 0x010002
	if (version == 0x010001)
		memcpy(ops, &pcm_src_ops, sizeof(snd_pcm_rate_old_ops_t));
	else
#endif
		*ops = pcm_src_ops;
	return 0;
}

int SND_PCM_RATE_PLUGIN_ENTRY(asrcrate) (unsigned int version, void **objp,
					   snd_pcm_rate_ops_t *ops)
{
	return pcm_src_open(version, objp, ops, 0);
}
