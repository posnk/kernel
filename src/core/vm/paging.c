/**
 * kernel/paging.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#include <stddef.h>

#include "core/paging.h"
#include "core/physmm.h"
#include "core/heap.h"
#include "core/core.h"
#include "core/impl.h"
#include "kernel/process.h"

page_dir_t *paging_active_dir;


/** 
 * @brief Used by the kernel heap manager to request a piece of new kernel heap
 * @param address 	The address to insert the new heap at.
 * @param size		The size of the heap block to create 
 */
size_t core_request_heap ( void *address, size_t size )
{	
	size_t size_counter;
	physaddr_t frame;

	/* Check whether paging has been set up yet */
	if (paging_active_dir == NULL)
		return 0;

	/* Iterate over the pages composing the new block*/
	for (size_counter = 0; size_counter < size; size_counter += PHYSMM_PAGE_SIZE) {
		/* Allocate a frame for the page */
		frame = physmm_alloc_frame();

		/* Check if there was enough memory to do this */
		if (frame == PHYSMM_NO_FRAME){
			core_oops("can't grow kernel heap: out of memory");
			break;
		}

		/* Create the page mapping */
		paging_map(address, frame, PAGING_PAGE_FLAG_RW);
		paging_tag(address, PAGING_PAGE_TAG_KERNEL_DATA);

		/* Bump the address counter */
		address = (void *) ( ((uintptr_t) address) + 
							((uintptr_t) PHYSMM_PAGE_SIZE) );
	}

	/* Return the number of bytes allocated */
	return size_counter;
}

void paging_handle_out_of_memory()
{
	core_oops("!!!!out of memory warning!!!!");
}


void paging_handle_fault(	void *	virt_addr, 
							void *	instr_ptr, 
							void *	state, 
							size_t	state_size, 
							int		present, 
							int		write, 
							int		user)
{
	uintptr_t addr = (uintptr_t) virt_addr;
	physaddr_t phys_addr;
	/* Handle page fault here, core runtime only needs to fix kernel stack */
	/* defer further handling to kernel implementation */
	if ((!present) && (addr >= 0xBFFF0000) && (addr < 0xBFFFB000)) {
		/* Task stack area, add more stack */
		virt_addr = (void *)(addr & ~PHYSMM_PAGE_ADDRESS_MASK);
		phys_addr = physmm_alloc_frame();		
		if (phys_addr == PHYSMM_NO_FRAME) {
			paging_handle_out_of_memory();
			phys_addr = physmm_alloc_frame();
		}
		//TODO: Assess potential for race conditions
		paging_map(virt_addr, phys_addr, PAGING_PAGE_FLAG_RW | 
										 PAGING_PAGE_FLAG_USER);
		paging_tag(virt_addr, PAGING_PAGE_TAG_USER_DATA);				
	} else 
		impl_page_fault(	virt_addr,
							instr_ptr, 
							state, 
							state_size, 
							present, 
							write, 
							user	);
}
