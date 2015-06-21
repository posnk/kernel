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
	
	// Account for any non-child references
	dest->refcount += src->refcount;
	
	// Free the merged-in area
	free( src );

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

iomm_range_t	*iomm_register_range ( physaddr_t start, physaddr_t end )
{
	
}
