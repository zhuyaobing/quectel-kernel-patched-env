/* Copyright (C) 2013-2014 OpenSynergy GmbH
 * This file is part of the OpenSynergy L2LV framework
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __VBD_LINK_H__
#define __VBD_LINK_H__

#include <linux/l2lv/l2lv_link.h>
#include <linux/l2lv/l2lv_server_links.h>
#include <linux/l2lv/vbd_server.h>
#include "vbd_defs.h"
#include "vbd_private_if.h"

struct l2lv_block_joblist;

typedef struct vbd_links_info {
    struct l2lv_links         *drv_links;
    struct l2lv_block_joblist *joblist;
} vbd_links_info;

typedef struct vbd_info {
    char part_name[VBD_MAX_PART_NAMELEN];
} vbd_info;

typedef struct vbd_infos {
    vbd_links_info *links_info;
    int	count;
    vbd_info *infos[VBD_MAX_BD];
} vbd_infos;

void vbd_init_links(vbd_links_info *links_info);
void vbd_destroy_links(vbd_links_info *links_info);
int  vbd_set_link(vbd_links_info *links_info, const _vbd_setlink_t *set_link);
int  vbd_remove_link(vbd_links_info *links_info, uint32_t link_id);

#endif /* __VBD_LINK_H__ */
