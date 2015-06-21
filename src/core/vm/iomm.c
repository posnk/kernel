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
 * @file core/vm/iomm.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <core/iomm.h>
#include <core/heap.h>

llist_t	iomm_area_list;

/** Matches an area tail-overlapping the param [param|i]tem|*/
int				 iomm_area_eoverlap_iterator( llist_t *_item, void *_param )
{
	iomm_area_t		*item  = (iomm_area_t *) _item;
	iomm_sparam_t	*param = (iomm_sparam_t *) _param;
	
	return  (_item != param->exclude) &&
			(item->start > param->start) &&
			(item->start <= param->end) &&
			(item->end > param->end);
}

/** Matches an area start-overlapping the param [item|p]aram|*/
int				 iomm_area_soverlap_iterator( llist_t *_item, void *_param )
{
	iomm_area_t		*item  = (iomm_area_t *) _item;
	iomm_sparam_t	*param = (iomm_sparam_t *) _param;
	
	return  (_item != param->exclude) &&
			(item->end >= param->start) && 
			(item->end < param->end) && 
			(item->start < param->start);
}

/** Matches an area that contains the range passed as param */
int				 iomm_area_contains_iterator( llist_t *_item, void *_param )
{
	iomm_area_t		*item  = (iomm_area_t *) _item;
	iomm_sparam_t	*param = (iomm_sparam_t *) _param;
	
	return (_item != param->exclude) &&
			(item->start <= param->start) && 
			(item->end >= param->end);
}

/** Matches an area contained within the range passed as param */
int				 iomm_area_contained_iterator( llist_t *_item, void *_param )
{
	iomm_area_t		*item  = (iomm_area_t *) _item;
	iomm_sparam_t	*param = (iomm_sparam_t *) _param;
	
	return (_item != param->exclude) &&
			(item->start >= param->start) && 
			(item->end <= param->end);
}
	
/**
 * Internal function, safely deletes an empty area
 * @param area		The area to delete
 * @param reclaim	Whether to reclaim the area as general purpose memory
 */ 
void			 iomm_delete_area ( iomm_area_t *area, int reclaim )
{
	
	assert ( area != NULL );
	
	// Check whether the area has refcount zero
	assert ( area->refcount == 0 );
	
	// If we were asked to reclaim the memory, return it to the physical mm
	if ( reclaim )
		physmm_free_range( area->start, area->end );
	
	// Free the merged-in area
	free( area );
}
	
/**
 * Internal function, merges two io memory areas
 * @param dest	The area to merge into
 * @param src	The area that will be destroyed by merging
 * @note		This function requires a lock on the iomm state
 */
void			 iomm_merge_areas ( iomm_area_t *dest, iomm_area_t *src )
{

	llist_t			*_work;
	llist_t			*_next;
	iomm_range_t	*rwork;
	iomm_map_t		*mwork;
	
	assert ( dest != NULL );
	assert ( src  != NULL );
	
	// Move all ranges to the destination area
	for ( _work = src->io_list.next; _work != &(src->io_list); _work = _next ) {
		
		// Acquire pointer to next range
		_next	= _work->next;
		
		// Cast work pointer to the right type
		rwork	= ( iomm_range_t * ) _work;
		
		// Update the area field on the range
		rwork->area = dest;
		
		// Unlink the range from its old area
		llist_unlink ( _work );
		
		// Add it to the new area
		llist_add_end( &(dest->io_list), _work );
		
		// Update the reference counters		
		src->refcount--;
		dest->refcount++;
		
	}
	
	// Move all mappings to the destination area
	for (	_work = src->map_list.next; 
			_work != &(src->map_list); 
			_work = _next ) {
		
		// Acquire pointer to next range
		_next	= _work->next;
		
		// Cast work pointer to the right type
		mwork	= ( iomm_map_t * ) _work;
		
		// Update the area field on the range
		mwork->area = dest;
		
		// Unlink the range from its old area
		llist_unlink ( _work );
		
		// Add it to the new area
		llist_add_end( &(dest->map_list), _work );
		
		// Update the reference counters
		src->refcount--;
		dest->refcount++;
		
	}
	
	// Free the merged-in area
	iomm_delete_area( src );

}
	
/**
 * Internal function, gets or creates an area containing the specified range
 * @param start The start of the range to find/create
 * @param end	The end of the range to find/create
 * @note		This function requires a lock on the iomm state
 * @return		An area containing the range
 */
iomm_area_t		*iomm_get_area ( physaddr_t start, physaddr_t end )
{
	iomm_area_t		*area, area2;
	iomm_sparam_t	 range_param;
	
	assert ( ( start & PHYSMM_PAGE_ADDRESS_MASK ) == 0 );
	assert ( ( end & PHYSMM_PAGE_ADDRESS_MASK )
	
	// Set up search parameters
	range_param.start	= start;
	range_param.end		= end;
	range_param.exclude = NULL;
	
	// Look for an existing area containing the range we are asked for
	area = (iomm_area_t *) llist_iterate_select(	&iomm_area_list, 
													iomm_area_contains_iterator,
													&range_param);
	
	// If we have found such an area, use it.
	if ( area != NULL )
		return area;
	
	// We have not. Look for an area overlapping with the range
	area = (iomm_area_t *) llist_iterate_select(	&iomm_area_list, 
													iomm_area_soverlap_iterator,
													&range_param);
		
	// If it exists, resize it to match our needs											
	if ( area != NULL ) 
		area->end = end;
		
	range_param.exclude = (llist_t *) area;
	
	// Look for an area overlapping with the range
	area2 = (iomm_area_t *) llist_iterate_select(	&iomm_area_list, 
													iomm_area_eoverlap_iterator,
													&range_param);

	// Handle tail overlapping areas 
	if ( area2 != NULL ) {
		
		// If we already had an area, merge it in, otherwise use the newly found
		// area
		if ( area != NULL ) 
			iomm_merge_areas( area, area2 );
		else {
			// Use the newly found area, resize it to make it include the new
			// range
			area = area2;
			area->start = start;
		}
		
		area2 = NULL;
		
		range_param.exclude = (llist_t *) area;
		
	}
	
	do {
	
		// Look for areas contained within this range
		area2 = (iomm_area_t *) llist_iterate_select(&iomm_area_list, 
												iomm_area_contained_iterator,
												&range_param);
												
		if ( area2 != NULL ) {
			
			// If we already had an area, merge it in, otherwise use the newly
			// found area
			if ( area != NULL ) 
				iomm_merge_areas( area, area2 );
			else {
				
				// Use the newly found area, resize it to make it include the
				// new range
				area = area2;
				area->start = start;
				area->end = end;
				range_param.exclude = (llist_t *) area;
				
			}
			
		}
		
	} while ( area2 != NULL );
	
	// Return the newly resized area
	if ( area != NULL )
		return area;
		
	// No area has been found that atleast overlaps the specified range, create
	// a new area...
	area = malloc( sizeof ( iomm_area_t ) );
	
	//TODO: Safely handle the out of memory error condition
	assert( area != NULL );
	
	// Fill its fields
	area->start		= start;
	area->end		= end;
	area->refcount	= 0;
	llist_create( &(area->io_list) );
	llist_create( &(area->map_list) );
	
	// Add the new area to the 
	llist_add_end( &iomm_area_list, (llist_t *) area );
	
	return area;
}

/** Matches any range that collides with the parameter*/
int				 iomm_range_collision_iterator( llist_t *_item, void *_param )
{
	iomm_range_t	*item  = (iomm_range_t *) _item;
	iomm_sparam_t	*param = (iomm_sparam_t *) _param;
	
	return  (_item != param->exclude) &&
			(item->start <= param->end) &&
			(item->end >= param->start);
}

iomm_range_t	*iomm_register_range ( physaddr_t start, physaddr_t end )
{
	iomm_area_t		*area;
	iomm_range_t	*range;
	iomm_sparam_t	 range_param;
	physaddr_t		 pstart, pend;
	
	// Calculate the page bounded start and end addresses
	//  Page bounded start is rounded down by masking the page index bits
	pstart	= start & ~PHYSMM_PAGE_ADDRESS_MASK;
	//	Page bounded end is rounded up
	pend	= ( end + PHYSMM_PAGE_ADDRESS_MASK ) & ~PHYSMM_PAGE_ADDRESS_MASK;
	
	// Get the area that will contain the region
	area = iomm_get_area( pstart, pend );
	
	// Set up the search parameters
	range_param.start	= start;
	range_param.end		= end;
	range_param.exclude	= NULL;
	
	// Check whether the range collides with any exisitng IO ranges
	range = (iomm_range_t *) llist_iterate_select(	&(area->io_list), 
												   iomm_range_collision_iterator
													&range_param);
	
	// If there is a collision, return NULL
	if ( range != NULL )
		return NULL;

	// Allocate memory for the range descriptor
	range = malloc( sizeof( iomm_range_t ) );
	
	// Check whether the allocation succeeded
	assert ( range != NULL );	
	
	// Fill the fields of the descriptor
	range->area		= area;
	range->start	= start;
	range->end		= end;
	range->refcount = 0;
	
	// Add the range to the area
	llist_add_end( &(area->io_list), range );
	
	// Bump the area's refcount
	area->refcoun++;
	
	return range;
}


