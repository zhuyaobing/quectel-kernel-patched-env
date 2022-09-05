/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc. All rights reserved.
 */
/**
   @file src_pair.h
   @brief helper functions for asrc driver
*/
/*
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

#ifndef ASRC_PAIR_H
#define ASRC_PAIR_H

#include <stdint.h>
#include <linux/mxc_asrc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int fd;
    int type;
    enum asrc_pair_index index;
    unsigned int channels;
    ssize_t in_period_frames;
    ssize_t out_period_frames;
    unsigned int in_rate;
    unsigned int out_rate;
    uint32_t buf_size;
    int buf_num;
    uint32_t num;
    uint32_t den;

    int is_converting;
} asrc_pair;

asrc_pair *asrc_pair_create(unsigned int channels, ssize_t in_period_frames,
        ssize_t out_period_frames, unsigned int in_rate, unsigned int out_rate, int type);

void asrc_pair_destroy(asrc_pair *pair);

void asrc_pair_get_ratio(asrc_pair *pair, uint32_t *num, uint32_t *den);

int asrc_pair_set_rate(asrc_pair *pair, ssize_t in_period_frames,
        ssize_t out_period_frames, unsigned int in_rate, unsigned int out_rate);

void asrc_pair_reset(asrc_pair *pair);

void asrc_pair_convert_s16(asrc_pair *pair, const int16_t *src, unsigned int src_frames,
        int16_t *dst, unsigned int dst_frames);

#ifdef __cplusplus
}
#endif

#endif
