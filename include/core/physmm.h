/******************************************************************************\
Copyright (C) 2015 Peter Bosch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file include/core/physmm.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __physmm__
#define __physmm__

#include "config.h"

#define PMAREA_TYPE_NONE		(0)
#define PMAREA_TYPE_USED		(1)
#define PMAREA_TYPE_INFO		(2)
#define PMAREA_TYPE_MTAB		(3)
#define PMAREA_TYPE_RESV		(4)
#define PMAREA_TYPE_MMIO		(5)
#define PMAREA_TYPE_FREE		(6)
#define PMAREA_TYPE_MASK		(7)
#define PMAREA_GET_TYPE(Flags)	(Flags & PMAREA_TYPE_MASK)
#define PMAREA_IS_FREE(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_FREE)
#define PMAREA_IS_USED(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_USED)
#define PMAREA_IS_INFO(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_INFO)
#define PMAREA_IS_MTAB(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_MTAB)
#define PMAREA_IS_RESV(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_RESV)
#define PMAREA_IS_MMIO(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_MMIO)
#define PMAREA_IS_NONE(Flags)	(PMAREA_GET_TYPE(Flags) == \
								 PMAREA_TYPE_NONE)

/**
 * Structure describing a continuous run of PM
 */
struct pmarea {

	/** Contains information about this region */
	int			flags;

	/** Link to the containing domain */
	struct pmdomain *	domain;

	/** Link to the previous physical memory area */
	struct pmarea *	next;

	/** Link to the next physical memory area */
	struct pmarea *	prev;

	/** Start address of this area */
	physaddr_t	start;

	/** End address of this area */
	physaddr_t	end;

	/** Reference count */
	int			refcount;

	/** Type specific information */
	union {
		struct {
			struct pmarea *areas;
			int	acnt;
		};

	};

};

/**
 * Describes a PM domain
 */
struct pmdomain {
	physaddr_t	start;
	physaddr_t	end;
	pmarea_t *	free_list;
	pmarea_t *	info_list;
	physaddr_t	framesize;
	int			infolen;
};

typedef struct pmarea pmarea_t;



//TODO: Adapt this constant to suit different physical address widths
#define PHYSMM_BITMAP_SIZE	(PHYSICAL_FRAME_COUNT/32)
#define PHYSMM_NO_FRAME		(0x00000001)

void physmm_set_bit(physaddr_t address);

void physmm_clear_bit(physaddr_t address);

void physmm_free_range(physaddr_t start, physaddr_t end);

void physmm_claim_range(physaddr_t start, physaddr_t end);

physaddr_t physmm_alloc_frame();

physaddr_t physmm_alloc_quadframe();

physaddr_t physmm_count_free();

void physmm_free_frame(physaddr_t address);

void physmm_init();

#endif
