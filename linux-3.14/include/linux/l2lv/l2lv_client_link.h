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
#ifndef _L2LV_CLIENT_LINK_H_
#define _L2LV_CLIENT_LINK_H_

#include <linux/l2lv/l2lv_link.h>

struct l2lv_link *l2lv_get_client_link(void);
int l2lv_client_open_channel(struct l2lv_channel *channel);
void l2lv_client_close_channel(struct l2lv_channel *channel);

#endif				/* _L2LV_CLIENT_LINK_H_ */
