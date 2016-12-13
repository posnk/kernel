/******************************************************************************\
Copyright (C) 2015,2016 Peter Bosch

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
 * @file include/arch/i386/paging.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __i386_paging__
#define __i386_paging__
#include "config.h"

typedef uint32_t pdir_t;

#define	PAGING_BIT_PRESENT	(1 << 0)
#define PAGING_BIT_WRITE	(1 << 1)
#define PAGING_BIT_USER		(1 << 2)
#define PAGING_BIT_WRTHROUGH	(1 << 3)
#define PAGING_BIT_NOCACHE	(1 << 4)
#define PAGING_BIT_ACCESSED	(1 << 5)
#define PAGING_BIT_DIRTY	(1 << 6)
#define PAGING_BIT_PAGESIZE	(1 << 7)
#define PAGING_BIT_GLOBAL	(1 << 8)
#define PAGING_PHYSADDR(PaD)	( ( ( pdir_t ) (PaD) ) & ~0xFFF )

#define PAGING_PAGEDIR_LENGTH	(1024)
#define PAGING_PAGETBL_LENGTH	(1024)

#define PAGING_RM_START		(0xFFC00000)
#define PAGING_RM_A_PTAB(IdX)	(PAGING_RM_START | ( (IdX)  << 12 ) )
#define PAGING_RM_A_PDIR	(PAGING_RM_A_PTAB(0xFFF))
#define PAGING_RM_P_PTAB(IdX)	((pdir_t *) PAGING_RM_A_PTAB(IdX))
#define PAGING_RM_P_PDIR	((pdir_t *) PAGING_RM_A_PDIR)
#define PAGING_ADDR_PTIDX(AdDr)	((((uintptr_t) (AdDr)) >> 12) & 0x3FF)
#define PAGING_ADDR_PDIDX(AdDr)	((((uintptr_t) (AdDr)) >> 22) & 0x3FF)
#define PAGING_ADDR_OFF(AdDr)	(((uintptr_t) (AdDr))&0xFFF)
#define PAGING_RM_PDIDX		(0x3FF)
#define PAGING_ST_PDIDX		(0x3FE)
#define PAGING_PHYSWIN_IDX(NuM)	(0x3FF - (NuM))
#define PAGING_PHYSWIN_PTR(NuM)	((void *)(0xFF800000 | ((0x3FF - (NuM)) << 12)))
#define PAGING_PHYSWIN_CNT	(16)

#endif
