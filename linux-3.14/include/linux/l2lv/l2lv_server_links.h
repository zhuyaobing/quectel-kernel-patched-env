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
#ifndef _L2LV_SERVER_LINKS_H_
#define _L2LV_SERVER_LINKS_H_

#include <linux/l2lv/l2lv_link.h>

/*
 * Collection of all links that the current server has.
 */
struct l2lv_links {
/* private: */
	uint32_t links_count;
	struct l2lv_link links[L2LV_MAX_CLIENTS];
};

struct l2lv_links *l2lv_server_get_links(void);

/*
 * Add and remove links to the list of links of the server.
 *
 * The l2lv server manages one link for every client. To keep track of the
 * links every link has to be added to the server explicitly after its
 * initialization.
 */
int l2lv_add_link(struct l2lv_links *links, unsigned int link_id,
		      const char *ip_name, const char *op_name);
void l2lv_remove_link(struct l2lv_links *links, unsigned int link_id);
struct l2lv_link *l2lv_get_link_by_id(struct l2lv_links *links,
					      unsigned int link_id);

/* For local link channels only! */
void l2lv_set_local_buffer(struct l2lv_channel *channel);
void l2lv_unset_local_buffer(struct l2lv_channel *channel);

#endif				/* _L2LV_SERVER_LINKS_H_ */
