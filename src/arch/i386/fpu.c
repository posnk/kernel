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
 * @file arch/i386/fpu.c
 *
 * Part of posnk kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 01-04-2014 - Created
 * 18-06-2015 - Commented
 * 18-06-2015 - Ported to pos-kernel, added GPL license header
 */

#include <stdint.h>
#include <string.h>
#include <arch/i386/x86.h>
#include <arch/i386/task_context.h>
#include <core/impl.h>

/** Aligned buffer for floating point store/load instructions */
uint8_t  i386_fpu_cs_buf[512] __attribute__((aligned(16)));

/** The last task that used the floating point module */
i386_task_context_t	*i386_fpu_context = NULL;

/** Whether floating point support has been enabled */
int		 i386_fpu_enabled = 0;

/**
 * Initialize the floating point processor support
 */
void i386_fpu_initialize()
{
	i386_fpu_enabled = 1;
}

/**
 * Internal function that enables the floating point module on the processor
 */ 
void i386_enable_fpu()
{
	uint32_t temp;
	
	// Clear the TaskSwitched flag, fetch CR4
	asm volatile("clts;mov %%cr4, %0" : "=r"(temp));
	
	// Enable  osfxsr and osxmmexcpt flags in CR4
	temp |= 3 << 9;
	
	// Restore CR4
	asm volatile("mov %0, %%cr4" :: "r"(temp));
}

/**
 * Internal function that disables the floating point module on the processor
 */ 
void i386_disable_fpu()
{
	uint32_t temp;
	
	// Fetch CR0
	asm volatile("mov %%cr0, %0" : "=r"(temp));
	
	// Set TaskSwitched flag
	temp |= 1 << 3;
	
	// Restore CR0
	asm volatile("mov %0, %%cr0" :: "r"(temp));
}

/**
 * Internal function that initializes the floating point module on the processor
 */ 
void i386_init_fpu()
{
	uint32_t c = 0x37F;
	asm volatile("fninit; fldcw %0":: "m"(c));
}

/**
 * Internal function that stores the floating point state
 */ 
void i386_fpu_save(i386_task_context_t *context)
{
	
	// Store the floating point state in the aligned buffer
	asm volatile("fxsave %0" : "=m" (i386_fpu_cs_buf));
	
	// Copy the aligned buffer to the target context object
	memcpy(context->fpu_state, i386_fpu_cs_buf, 512);
}

/**
 * Internal function that loads the floating point state
 */ 
void i386_fpu_load(i386_task_context_t *context)
{
	
	// Copy the target context object to the aligned buffer
	memcpy(i386_fpu_cs_buf, context->fpu_state, 512);
	
	// Load the floating point state from the aligned buffer
	asm volatile("fxrstor %0" : "=m" (i386_fpu_cs_buf));
	
}

/**
 * Context switch handler for the floating point module
 */
void i386_fpu_on_cs()
{
	// If the FPU is enabled, disable it so that a FPU operation in the next 
	// process will generate an illegal instruction exception
	if (i386_fpu_enabled)
		i386_disable_fpu();
}

/**
 * Illegal instruction handler for the floating point module, implements lazy
 * context switching for the FPU
 * 
 * @return Whether we handled the exception
 */
int i386_fpu_handle_ill()
{
	i386_task_context_t  *current_context;
	
	// If FPU support is not enabled, do not eat the exception and return
	if(!i386_fpu_enabled)
		return 0;
	
	// Enable the floating point unit
	i386_enable_fpu();
	
	// Fetch the processor context for this task from the OS implementation
	current_context = (i386_task_context_t *) impl_get_cpu_context();
	
	// Check whether the FPU already held this task's context, if so: return
	if ( i386_fpu_context == current_context )
		return 1;
		
	// We are not the last task that accessed the FPU, perform a FPU context 
	// switch
	
	
	// Check whether any task had used the FPU before us
	if ( i386_fpu_context ) {
		
		// If so, store the FPU context for that task
		
		//TODO: Handle dangling pointer on task termination!
		i386_fpu_save( i386_fpu_context );
		
	}
	
	// Set the FPU context tracker to the current task
	i386_fpu_context = current_context;
	
	// Check whether this task had previously used the FPU
	if ( !i386_fpu_context->fpu_used ) {
		
		// If not, initialize the FPU
		i386_init_fpu();
		
		// and set the FPU used flag on this context
		i386_fpu_context->fpu_used = 1;
		
		// Signal that we have handled the exception
		return 1;		
	}
	
	// This task had previously used the FPU so we need to restore that state
	i386_fpu_load( i386_fpu_context );
	
	// Signal that we have handled the exception
	return 1;
}
