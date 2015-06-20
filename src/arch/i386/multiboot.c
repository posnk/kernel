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
 * @file arch/i386/multiboot.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"

uint32_t i386_multiboot_memprobe(multiboot_info_t* mbt)
{
	uint32_t available = 0;
	multiboot_memory_map_t*		mmapr	= (multiboot_memory_map_t*) (((uintptr_t)mbt->mmap_addr) + 0xC0000000) ;
	multiboot_memory_map_t*		mmap 	= mmapr;
	
	physmm_free_range(0x400000, 0x800000);
	
	while(((uintptr_t)mmap) < (((uintptr_t)mmapr) + mbt->mmap_length)) {
		// TODO : Verify page alignment of mmap entries
		if (mmap->base_addr_low >= 0x100000) {
			
			if ((mmap->type == 1) && (mmap->base_addr_high == 0)) {
				if (mmap->length_high != 0)
					mmap->length_low = 0xFFFFFFFF - mmap->base_addr_low;
				available += mmap->length_low;
				physmm_free_range(	mmap->base_addr_low, 
									mmap->base_addr_low+mmap->length_low);
			}
			
		}
			mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}
	return available;
}

void i386_init_reserve_modules(multiboot_info_t *mbt)
{
	unsigned int i;
	multiboot_module_t *modules = (multiboot_module_t *) 
									(mbt->mods_addr + 0xC0000000);
	physmm_claim_range((physaddr_t)mbt->mods_addr, ((physaddr_t)mbt->mods_addr) + sizeof(multiboot_module_t)*(mbt->mods_count));
	for (i = 0; i < mbt->mods_count; i++){
		physmm_claim_range((physaddr_t)modules[i].string, (physaddr_t)modules[i].string + 80);
		physmm_claim_range((physaddr_t)modules[i].mod_start, (physaddr_t)modules[i].mod_end);
	}
}
