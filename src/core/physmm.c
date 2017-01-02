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
 * @file core/physmm.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <string.h>
#include <assert.h>
#include "core/physmm.h"

#define pmsize( Info )	( (Info)->end - (Info)->start )

pmdomain_t physmm_core_domain;

pmarea_t *physmm_alloc_desc( pmdomain_t *domain )
{
	int pos;
	pmarea_t *cur, *res;

	assert( domain != NULL );

	for ( cur = domain->info_list; cur != NULL; cur = cur->next ) {
		if ( cur->acnt < domain->infolen )
			break;
	}

	if ( cur == NULL )
		return NULL;

	for ( pos = 0; pos < domain->infolen; pos++ ) {
		if ( PMAREA_IS_NONE( cur->areas[ pos ]->flags ) )
			break;
	}

	if ( pos == domain->infolen )
		return;

	cur->acnt++;

	cur = cur->areas+pos;

	cur->flags &=~ PMAREA_TYPE_MASK;
	cur->flags |=  PMAREA_TYPE_RSVD;

	return cur;
}

void physmm_create_infoframe( pmarea_t *area )
{
	pm_domain_t *domain;
	pmarea_t 	*info;
	pmarea_t	 tmpinfo;
	physaddr_t	 size;
	bmap_t		*bmap;
	int oldflags;

	assert( area != NULL );

	/* Extract domain */
	domain = area->domain;

	assert( domain != NULL );

	size = pmsize( area );

	/* If we're asked to create an infoframe on a larger area */
	/* split its start off to form the infoblock */
	if ( size > domain->framesize ) {
		tmpinfo.flags = PMAREA_TYPE_INFO;
		tmpinfo.domain= area->domain;
		tmpinfo.start = area->start;
		tmpinfo.end	  = area->end;
		area->start = area->start + domain->framesize;
		info = &tmpinfo;
	} else
		info = area;

	/* Page in the info frame */	
	physmm_map_infoframe( info );

	/* Clear infoframe */
	memset( info->areas, 0, sizeof( pmarea_t ) * domain->infolen );
	info->acnt = 0;

	/* Copy infoframe descriptor onto itself */
	if ( info != area ) {
		info->areas[0] = tmpinfo;
		info->acnt = 1;
		info = info->areas;	
	}	

	/* Link it into the info frame list */
	info->next = domain->info_list
	info->prev = 0;
	domain->info_list = info;

}

void physmm_create_domain( )
{
	pmdomain_t *domain;
	domain->free_list = NULL;
	domain->info_list = NULL;
	domain->infolen = domain->framesize / sizeof( pmarea_t );
}

void physmm_init( )
{
}

uint32_t physmm_bitmap[PHYSMM_BITMAP_SIZE];

void physmm_set_bit(physaddr_t address)
{
	address >>= 12;
	physmm_bitmap[(address >> 5) & 0x7FFF] |= 1 << (address & 0x1F);
}

void physmm_clear_bit(physaddr_t address)
{
	address >>= 12;
	physmm_bitmap[(address >> 5) & 0x7FFF] &= ~(1 << (address & 0x1F));
}

void physmm_free_range(physaddr_t start, physaddr_t end)
{
	physaddr_t counter;
	assert(start < end);
	for (counter = start; counter < end; counter += ARCH_PAGE_SIZE){
		physmm_free_frame(counter);
	}
}

void physmm_claim_range(physaddr_t start, physaddr_t end)
{
	physaddr_t counter;
	assert(start < end);
	for (counter = start; counter < end; counter += ARCH_PAGE_SIZE){
		physmm_clear_bit(counter);
	}
}

physaddr_t physmm_alloc_frame()
{
	//TODO: Lock physmm_bitmap
	physaddr_t counter, bit_counter;
	for (counter = 0; counter < PHYSMM_BITMAP_SIZE; counter++) {
		if (physmm_bitmap[counter] == 0)
			continue;
		for (bit_counter = 0; bit_counter < 32; bit_counter++) {
			if (physmm_bitmap[counter] & (1 << bit_counter)) {
				counter <<= 5;
				counter |= bit_counter;
				counter <<= 12;
				physmm_clear_bit(counter);
				//TODO: Release physmm_bitmap
				return counter;
			}
		}
	}	
	//TODO: Release physmm_bitmap
	return PHYSMM_NO_FRAME;
}

physaddr_t physmm_alloc_quadframe()
{
	//TODO: Lock physmm_bitmap
	physaddr_t counter, bit_counter;
	for (counter = 0; counter < PHYSMM_BITMAP_SIZE; counter++) {
		if (physmm_bitmap[counter] == 0)
			continue;
		for (bit_counter = 0; bit_counter < 32; bit_counter+=4) {
			if ( ( physmm_bitmap[counter] & 
			     (0xF << bit_counter) ) == 0xF ) {
				counter <<= 5;
				counter |= bit_counter;
				counter <<= 12;
				physmm_clear_bit(counter+ARCH_PAGE_SIZE*0);
				physmm_clear_bit(counter+ARCH_PAGE_SIZE*1);
				physmm_clear_bit(counter+ARCH_PAGE_SIZE*2);
				physmm_clear_bit(counter+ARCH_PAGE_SIZE*3);
				//TODO: Release physmm_bitmap
				return counter;
			}
		}
	}
	//TODO: Release physmm_bitmap
	return PHYSMM_NO_FRAME;
}

physaddr_t physmm_count_free()
{
	//TODO: Lock physmm_bitmap
	physaddr_t counter, bit_counter, result;
	result = 0;
	for (counter = 0; counter < PHYSMM_BITMAP_SIZE; counter++) {
		if (physmm_bitmap[counter] == 0)
			continue;
		for (bit_counter = 0; bit_counter < 32; bit_counter++) {
			if (physmm_bitmap[counter] & (1 << bit_counter))
				result += ARCH_PAGE_SIZE;
		}
	}
	//TODO: Release physmm_bitmap
	return result;
}

void physmm_free_frame(physaddr_t address)
{
	physmm_set_bit(address);
}

void physmm_init(){
	memset(physmm_bitmap, 0, 32768 * sizeof(uint32_t));
}


