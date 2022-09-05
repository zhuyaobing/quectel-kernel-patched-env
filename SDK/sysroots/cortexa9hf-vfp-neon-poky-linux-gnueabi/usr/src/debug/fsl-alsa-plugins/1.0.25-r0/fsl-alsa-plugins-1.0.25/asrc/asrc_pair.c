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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/mxc_asrc.h>

#include "asrc_pair.h"

#define ASRC_DEVICE     "/dev/mxc_asrc"
#define DMA_MAX_BYTES   (32768)

#define LINEAR_RATE         (20)
#define LINEAR_PITCH_BITS   (16)
#define LINEAR_PITCH        (1 << LINEAR_PITCH_BITS)

static uint32_t get_max_divider(uint32_t x, uint32_t y)
{
    uint32_t t;

    while(y != 0)
    {
        t = x % y;
        x = y;
        y = t;
    }

    return x; 
}

static void calculate_num_den(asrc_pair *pair)
{
    uint32_t div;

    div = get_max_divider(pair->in_rate, pair->out_rate);
    pair->num = pair->in_rate / div;
    pair->den = pair->out_rate / div;
}

static int asrc_start_conversion(asrc_pair *pair)
{
    int err;

    if (pair->is_converting)
        return 0;

    err = ioctl(pair->fd, ASRC_START_CONV, &pair->index);
    if (err < 0)
        fprintf(stderr, "Unable to start ASRC converting %d\n", pair->index);

    pair->is_converting = 1;
    return err;
}

static int asrc_stop_conversion(asrc_pair *pair)
{
    int err;

    if (!pair->is_converting)
        return 0;

    err = ioctl(pair->fd, ASRC_STOP_CONV, &pair->index);
    if (err < 0)
        fprintf(stderr, "Unable to stop ASRC converting %d\n", pair->index);

    pair->is_converting = 0;
    return err;
}

static void get_dma_buffer_segments(unsigned int channels, uint32_t frames, uint32_t *seg_size, uint32_t *seg_num)
{
    int num = 1;
    uint32_t frame_bytes = frames << 1;
    uint32_t seg_bytes = frame_bytes;
    uint32_t alignment = channels << 1;

    while (seg_bytes > DMA_MAX_BYTES)
    {
        num++;
        seg_bytes = (frame_bytes + (alignment * num - 1)) / num;
        seg_bytes = seg_bytes - seg_bytes % alignment;
    }

    *seg_size = seg_bytes;
    *seg_num = num;
}

asrc_pair *asrc_pair_create(unsigned int channels, ssize_t in_period_frames,
        ssize_t out_period_frames, unsigned int in_rate, unsigned int out_rate, int type)
{
    int fd;
    int err;
    struct asrc_req req;
    struct asrc_config config;
    asrc_pair *pair = NULL;
    uint32_t dma_buffer_size;
    uint32_t buf_num;

    fd = open(ASRC_DEVICE, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Unable to open device %s\n", ASRC_DEVICE);
        goto end;
    }

    req.chn_num = channels;
    if ((err = ioctl(fd, ASRC_REQ_PAIR, &req)) < 0)
    {
        fprintf(stderr, "Req ASRC pair failed\n");
        goto close_fd;
    }

    get_dma_buffer_segments(channels, in_period_frames, &dma_buffer_size, &buf_num);

    config.pair = req.index;
    config.channel_num = req.chn_num;
    config.dma_buffer_size = dma_buffer_size;
    config.input_sample_rate = in_rate;
    config.output_sample_rate = out_rate;
    config.buffer_num = buf_num;
    config.input_word_width = ASRC_WIDTH_16_BIT;
    config.output_word_width = ASRC_WIDTH_16_BIT;
    config.inclk = INCLK_NONE;
    config.outclk = OUTCLK_ASRCK1_CLK;

    if ((err = ioctl(fd, ASRC_CONFIG_PAIR, &config)) < 0)
    {
        fprintf(stderr, "%s: Config ASRC pair %d failed\n", __func__, req.index);
        goto release_pair;
    }

    pair = calloc(1, sizeof(*pair));

    if (!pair)
        goto release_pair;

    pair->fd = fd;
    pair->type = type;
    pair->index = req.index;
    pair->channels = channels;
    pair->in_rate = in_rate;
    pair->out_rate = out_rate;
    pair->in_period_frames = in_period_frames;
    pair->out_period_frames = out_period_frames;
    pair->buf_size = dma_buffer_size;
    pair->buf_num = buf_num;
    calculate_num_den(pair);
    
    goto end;

release_pair:
    ioctl(fd, ASRC_RELEASE_PAIR, &req.index);

close_fd:
    close(fd);

end:
    return pair;
}

void asrc_pair_destroy(asrc_pair *pair)
{
    asrc_stop_conversion(pair);

    ioctl(pair->fd, ASRC_RELEASE_PAIR, &pair->index);
    close(pair->fd);
    
    free (pair);
}

void asrc_pair_get_ratio(asrc_pair *pair, uint32_t *num, uint32_t *den)
{
    *num = pair->num;
    *den = pair->den;
}

int asrc_pair_set_rate(asrc_pair *pair, ssize_t in_period_frames,
        ssize_t out_period_frames, unsigned int in_rate, unsigned int out_rate)
{
    struct asrc_config config;
    int err;
    uint32_t dma_buffer_size;
    uint32_t buf_num;
    int is_converting;

    if (in_rate == pair->in_rate && out_rate == pair->out_rate &&
            in_period_frames == pair->in_period_frames && out_period_frames == pair->out_period_frames)
        return 0;

    is_converting = pair->is_converting;
    asrc_stop_conversion(pair);

    get_dma_buffer_segments(pair->channels, in_period_frames, &dma_buffer_size, &buf_num);

    config.pair = pair->index;
    config.channel_num = pair->channels;
    config.dma_buffer_size = dma_buffer_size;
    config.input_sample_rate = in_rate;
    config.output_sample_rate = out_rate;
    config.buffer_num = buf_num;
    config.input_word_width = ASRC_WIDTH_16_BIT;
    config.output_word_width = ASRC_WIDTH_16_BIT;
    config.inclk = INCLK_NONE;
    config.outclk = OUTCLK_ASRCK1_CLK;

    if ((err = ioctl(pair->fd, ASRC_CONFIG_PAIR, &config)) < 0)
        fprintf(stderr, "%s: Config ASRC pair %d failed\n", __func__, pair->index);
    else
    {
        pair->buf_size = dma_buffer_size;
        pair->buf_num = buf_num;
        pair->in_rate = in_rate;
        pair->out_rate = out_rate;
        pair->in_period_frames = in_period_frames;
        pair->out_period_frames = out_period_frames;
        calculate_num_den(pair);
    }

    if (is_converting)
        asrc_start_conversion(pair);

    return err;
}

void asrc_pair_reset(asrc_pair *pair)
{
}

static void linear_pad_s16(asrc_pair *pair, int16_t *samples, int frames)
{
    unsigned int ch = pair->channels;
    unsigned int c;
    int i;

    int src_frames = frames * LINEAR_RATE;
    int dst_frames = frames * (LINEAR_RATE + 1);
    int16_t *src, *s, *d;
    int32_t pos;
    int32_t step = LINEAR_PITCH * (src_frames - 1) / (dst_frames - 1);

    src = (int16_t *) malloc((dst_frames << 1) * ch);

    /* first, copy samples to src buffer */
    memcpy(src, samples, (src_frames << 1) * ch);

    for (c = 0; c < ch; c++)
    {
        pos = 0;
        s = src + c;
        d = samples + c;
        for (i = 0; i < dst_frames; i++)
        {
            *d = ((LINEAR_PITCH - pos) * (*s) + pos * (*(s + ch))) >> LINEAR_PITCH_BITS;
            d += ch;
            pos += step;
            if (pos >= LINEAR_PITCH)
            {
                pos -= LINEAR_PITCH;
                s += ch;
            }
        }
    }

    free (src);
}

void asrc_pair_convert_s16(asrc_pair *pair, const int16_t *src, unsigned int src_frames,
        int16_t *dst, unsigned int dst_frames)
{
    struct asrc_convert_buffer buf_info;
    int err;
    unsigned int src_left = src_frames << 1;
    unsigned int dst_left = dst_frames << 1;
    char *s = (void *)src;
    char *d = (void *)dst;
    unsigned int in_len, out_len;
    int frames;
    int16_t *samples;

    asrc_start_conversion(pair);

    while (src_left > 0)
    {
	if (src_left > pair->buf_size) {
		in_len = pair->buf_size;
		out_len = dst_left * in_len/src_left;
	} else {
		in_len = src_left;
		out_len = dst_left;
	}

        buf_info.input_buffer_vaddr = s;
        buf_info.input_buffer_length = in_len;
        buf_info.output_buffer_vaddr = d;
        buf_info.output_buffer_length = out_len;

        if ((err = ioctl(pair->fd, ASRC_CONVERT, &buf_info)) < 0)
            fprintf(stderr, "%s: Convert ASRC pair %d failed, [%p][%d][%p][%d]\n", __func__,
                    pair->index, buf_info.input_buffer_vaddr, buf_info.input_buffer_length,
                    buf_info.output_buffer_vaddr, buf_info.output_buffer_length);

        s += in_len;
        src_left -= in_len;
        d += buf_info.output_buffer_length;
        dst_left -= buf_info.output_buffer_length;
        //printf("[%d/%d]\n", buf_info.output_buffer_length, out_len);
    }

    if (dst_left > 0)
    {
        frames = (dst_left >> 1) / pair->channels;
        /* we use LINEAR_RATE * N frames to generate (LINEAR_RATE+1)*N frames */
        samples = (int16_t *)d - frames * LINEAR_RATE * pair->channels;
        if (frames > 0 && samples >= dst)
        {
            /* try insert samples by linear alg */
            linear_pad_s16(pair, samples, frames);
        }
    }
}
