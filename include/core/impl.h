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
 * @file core/impl.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __core_impl__
#define __core_impl__
#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Handles a page fault
 * @param virt_addr The address which was being accessed when the fault occurred
 * @param instr_ptr The address which was being executed when the fault occurred
 * @param context	The processor context when the fault occurred
 * @param present	Whether the page being accessed was present
 * @param write		Whether the faulting access was a write operation
 * @param user		Whether the processor was executing user-level code
 */
void impl_page_fault(	void *	virt_addr, 
						void *	instr_ptr, 
						void *	context, 
						int		present, 
						int		write, 
						int		user);

/**
 * Returns the processor context for the currently executing task.
 * @returns A pointer to the context, guaranteed to be invariant across the 
 * 			entire runtime of the process.
 */ 
void *impl_get_cpu_context( void );

#ifdef __cplusplus
}
#endif

#endif
