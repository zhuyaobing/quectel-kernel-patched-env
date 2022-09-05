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

#ifndef _L2LV_HYP_SUPPORT_H_
#define _L2LV_HYP_SUPPORT_H_

#ifdef CONFIG_P4

#include "p4.h"
#include "vm.h"

typedef P4_spin_t hyp_spin_t;

#define hyp_flush_dcache_range(start, size) p4_flush_dcache_range((start), (size))
#define hyp_spin_lock(hyp_spinlock)         p4_spin_lock(hyp_spinlock)
#define hyp_spin_unlock(hyp_spinlock)       p4_spin_unlock(hyp_spinlock)
#define hyp_spin_init(hyp_spinlock)         p4_spin_init(hyp_spinlock)

#else /* CONFIG_P4 */

typedef int hyp_spin_t;

#define hyp_flush_dcache_range(start, size)

#define hyp_spin_lock(hyp_spinlock)

#define hyp_spin_unlock(hyp_spinlock)

#define hyp_spin_init(hyp_spinlock)

#endif /* CONFIG_P4 */

#endif
