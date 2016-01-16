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
 * @file include/core/paging.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __core_paging__
#define __core_paging__
#include "config.h"
#include <status.h>

#define PAGE_FLAG_IMPL1		(1 << 31)
#define PAGE_FLAG_NOCACHE	(32)
#define PAGE_FLAG_GLOBAL	(16)
#define PAGE_PERM_U		(8)
#define	PAGE_PERM_R		(4)
#define PAGE_PERM_W		(2)
#define PAGE_PERM_X		(1)
#define PAGE_PHYS_UNCHANGED	(1)

/**
 * @brief Maps a virtual address to a physical address in the specified dir
 * @param dir	The physical address of the directory
 * @param va	The virtual address to map
 * @param pa	The physical address to map
 * @param flags	The options
 * @return	The physical address previously mapped at that address
 */
physaddr_t	paging_map(	physaddr_t	dir, 
				void *		va, 
				physaddr_t	pa,
				int		flags,
				STPD );

/**
 * @brief	Acquires a virtual address window to a frame
 * @param pa	The physical address to map
 * @return	A pointer to a virtual address mapped to the window
 */
void *		paging_mkwindow(	physaddr_t pa,
					STPD	);

/**
 * @brief	Releases a virtual address window
 * @param va	The virtual address of the window to release
 */
void		paging_rmwindow(	void *va,
					STPD	);

/**
 * @brief Unmaps a physical address from an virtual address
 * @param dir	The directory to operate on
 * @param va	The virtual address to unmap
 * @return	The physical address previously mapped at that address
 */
physaddr_t	paging_unmap(	physaddr_t dir, 
				void *va,
				STPD );

/**
 * @brief Sets a page not present and change the physical address
 * @param dir	The directory to operate on
 * @param va	The virtual address to modify
 * @param pa	The new physical address to set ( PAGE_PHYS_UNCHANGED to \
 *		preserve the physical address )
 * @return	The physical address previously mapped at that address
 */
physaddr_t	paging_swmap(	physaddr_t dir, 
				void *va, 
				physaddr_t pa,
				STPD  );

/**
 * @brief Clones the global part of a page directory
 * @param dir	The directory to clone
 * @return	The new page directory.
 */
physaddr_t	paging_newdir(	physaddr_t dir,
				 STPD	);

/**
 * @brief Destroys a page directory
 * @param dir	The directory to destroy
 */
void		paging_freedir(	physaddr_t dir,
				STPD	);

/** 
 * @brief Returns the current page directory
 * @return	The current page directory
 */
physaddr_t	paging_getdir( void );

/**
 * @brief Gets the physical address for a virtual address
 * @return	The physical address
 */
physaddr_t	paging_getphys( physaddr_t	dir,
				void *		addr,
				int		flags,
				STPD );

#endif
