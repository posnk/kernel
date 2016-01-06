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
#include <string.h>
#include <assert.h>
#include "core/physmm.h"

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
