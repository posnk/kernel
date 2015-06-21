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
 * @file core/iomm.h
 *
 * IO Memory manager header
 * The IO memory manager provides a clean way to access IO or otherwisely fixed
 * blocks of physical address space, it wraps around the heap manager, physical
 * memory manager and the paging system
 * 
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __core_iomm__
#define __core_iomm__

#include <stdint.h>
#include <core/paging.h>
#include <core/physmm.h>

#include "config.h"

/**
 * Describes an area of physical memory that has been reserved for memory-mapped
 * input output, on paging architectures this will be a range of frames reserved
 * for IO
 */
typedef struct {
	/** Linked list link fields */
	llist_t		link;
	/** Physical start address of the IO area */
	physaddr_t	start;
	/** Physical end address of the IO area */
	physaddr_t	end;
	/** Linked list of IO ranges within this area */
	llist_t		io_list;
	/** Linked list of mappings to this area */
	llist_t		map_list;
	/** Reference counter */
	uint32_t	refcount;	
} iomm_area_t;

/**
 * Describes a range of IO space within an area, the atomic memory block this 
 * manager operates on.
 */
typedef struct {
	/** Linked list link fields */
	llist_t				 link;
	/** Pointer to the IO area this range belongs to */
	iomm_area_t			*area;
	/** Physical start address of the IO area */
	physaddr_t			 start;
	/** Physical end address of the IO area */
	physaddr_t			 end;
	/** Reference counter */
	uint32_t			 refcount;
} iomm_range_t;

/**
 * Describes a mapping of IO space to virtual space
 */
typedef struct {
	/** Linked list link fields */
	llist_t				 link;
	iomm_area_t			*area;
	uintptr_t			 vstart;
	page_dir_t			*pagedir;
	void				*userinfo;
	/** Reference counter */
	uint32_t			 refcount;
} iomm_map_t;

/**
 * Describes a reference of IO space from virtual space
 */
typedef struct {
	/** Linked list link fields */
	llist_t				 link;
	iomm_range_t		*range;
	iomm_map_t			*map;
	uintptr_t			 vstart;
	void				*userinfo;
} iomm_ref_t;

/**
 * Internal structure, used for parameter passing to iterator functions
 */
typedef struct {
	physaddr_t			 start;
	physaddr_t			 end;
	llist_t				*exclude;
} iomm_sparam_t;

/**
 * Registers a range of IO memory
 * @param start The start of the IO memory range
 * @param end	The end of the IO memory range
 * @return		A reference to the IO memory area
 */
iomm_range_t	*iomm_register_range ( physaddr_t start, physaddr_t end );

/**
 * @param range		The IO memory range to map
 * @param vstart	The virtual address to map it to
 */
void			*iomm_map_range ( iomm_range_t *range, void *target );

#endif
