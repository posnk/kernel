/**
 * kernel/dlheapmm.c
 *
 * Allows dlmalloc to be used as kernel allocator
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 14-07-2014 - Created
 */

#include "core/physmm.h"
#include "core/core.h"
#include <stdint.h>
#include <string.h>

void* memalign(size_t,size_t);
void* malloc(size_t);
void  free(void*);

/**
 * Pointer to the top of the heap
 */
void       	*heapmm_top_of_heap;

/**
 * Initializes the heap memory manager
 * @param heap_start The start of the heap
 */
void  coremm_init(void *heap_start, size_t size)
{
	//TODO : Do not throw away initial heap
	heapmm_top_of_heap = (void *) ( ((uintptr_t)heap_start) + ((uintptr_t) size) );
}

/**
 * 
 */
void dlheapmm_abort()
{	
	core_panic("heap allocator abort");
}

/**
 * Internal function
 * Requests more memory and adds it to the free block table,
 * then cleans up low space markers
 */
void * dlheapmm_sbrk ( size_t size )
{
	void *old = heapmm_top_of_heap;
	if (size == 0) {
		return heapmm_top_of_heap;
	} else if (size > 0) {
		size = core_request_heap ( heapmm_top_of_heap, size );

		/* Update top of heap pointer */
		heapmm_top_of_heap = ( void * ) ( ((uintptr_t)heapmm_top_of_heap) + ((uintptr_t)size));

		return old;
	} else {
		//TODO: WARN
		return (void *) -1;
	}
}
