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
 * @file core/i386/paging.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <assert.h>
#include <status.h>
#include "core/paging.h"
#include "core/i386/paging.h"
#include "arch/i386/archlib.h"
#include "core/physmm.h"

#ifdef	OPT_no_paging

void *		paging_mkwindow(	physaddr_t pa,
					STPD	)
{
	STRET( ( void * ) ( uintptr_t ) pa );
}

void		paging_rmwindow(	void *va,
					STPD	)
{
	STRETV;
}

#else

void *		paging_mkwindow(	physaddr_t pa,
					STPD	)
{
	pdir_t *pt;
	pdir_t	pe;
	int	pw;
	void *	va;

	pt = PAGING_RM_P_PTAB( PAGING_ST_PDIDX );

	for(	pw = 0;	pw < PAGING_PHYSWIN_CNT; pw++ ) {
		pe = pt[ PAGING_PHYSWIN_IDX( pw ) ];
		if ( pe & PAGING_BIT_PRESENT )
			continue;
		pe = 	PAGING_BIT_PRESENT	| 
			PAGING_BIT_WRITE	|
			PAGING_BIT_GLOBAL	|
			PAGING_PHYSADDR( pa );
		pt[ PAGING_PHYSWIN_IDX( pw ) ] = pe;

		va = PAGING_PHYSWIN_PTR( pw );

		i386_invtlb_addr( va );

		STRET( va );	

	}
	
	STERR( (void *)0, STCODE_NORES, 
		"No physical address space windows left" );
}

void		paging_rmwindow(	void *va,
					STPD	)
{
	int	pti, pdi;
	pdir_t *pt;
	
	pti = PAGING_ADDR_PTIDX( va );
	pdi = PAGING_ADDR_PDIDX( va );

	assert( pdi == PAGING_ST_PDIDX );
	assert( PAGING_PHYSWIN_IDX( pti ) >= PAGING_PHYSWIN_CNT );
	
	pt = PAGING_RM_P_PTAB( PAGING_ST_PDIDX );
	pt[ pti ] &= ~PAGING_BIT_PRESENT;

	STRETV;
}

#endif

physaddr_t	paging_getdir( void )
{

	return ( physaddr_t ) ( i386_get_cr3( ) & 0xFFFFFFF0 );

}

physaddr_t	paging_getphys(	physaddr_t dir,
				void 	*va,
				int	flags,
				STPD	)
{
	char *		ste;
	int		stc, pdi, pti;
	physaddr_t	cur_dir;
	pdir_t *	pd;
	pdir_t *	pt;
	pdir_t		pde = 0, pte = 0;

	cur_dir = paging_getdir();
	
	if ( cur_dir != dir ) {
		pd = paging_mkwindow( dir, STPC );
		if ( STPF ) {
		
			ste = 	"No physical address space window available for"
				" page directory";
			stc = STCODE_NORES;
			goto _bail_no_pdwnd;

		}
	} else 
		pd = PAGING_RM_P_PDIR;

	pdi = PAGING_ADDR_PDIDX( va );
	pti = PAGING_ADDR_PTIDX( va );

	pde = pd[ pdi ];

	if ( !( pde & PAGING_BIT_PRESENT ) ) {
		
		ste = "Page directory is marked not present";
		stc = STCODE_NOENT;
		goto _bail_tab_notpres;

	}

	if ( 	( flags & PAGE_PERM_U ) && (
		(!( pde & PAGING_BIT_USER )) ||
		(( flags & PAGE_PERM_W ) &&
		!( pde & PAGING_BIT_WRITE )) ) ) {

		ste = "Requested access denied";
		stc = STCODE_NOPERM;
		goto _bail_tab_noperm;
	}

	if ( cur_dir != dir ) {

		pt = paging_mkwindow( PAGING_PHYSADDR(pde), STPC );
		if ( STPF ) {
			
			ste =	"No physical address space window available for"
				" page table";
			stc = STCODE_NORES;
			goto _bail_no_ptwnd;

		}

	} else
		pt = PAGING_RM_P_PTAB( pdi );
	
	pte = pt[ pti ];

	if ( !( pte & PAGING_BIT_PRESENT ) ) {
		ste = "Page is marked not present";
		stc = STCODE_NOENT;
		goto _bail_pte_notpres;
	}
	
	if ( 	( flags & PAGE_PERM_U ) && (
		(!( pte & PAGING_BIT_USER )) ||
		(( flags & PAGE_PERM_W ) &&
		!( pte & PAGING_BIT_WRITE )) ) ) {

		ste = "Requested access denied";
		stc = STCODE_NOPERM;
		goto _bail_pte_noperm;
	}


	if ( cur_dir != dir ) {
		paging_rmwindow( pt, STPC );
		paging_rmwindow( pd, STPC );
	}

	return PAGING_PHYSADDR( pte ) | PAGING_ADDR_OFF( va );

_bail_pte_noperm:
_bail_pte_notpres:

	if ( cur_dir != dir )
		paging_rmwindow( pt, STPC );

_bail_no_ptwnd:
_bail_tab_noperm:
_bail_tab_notpres:
	
	if ( cur_dir != dir )
		paging_rmwindow( pt, STPC );

_bail_no_pdwnd:

	STERR( PAGING_PHYSADDR(pde), stc, ste );
	
}

physaddr_t      paging_setpe(	physaddr_t dir,
				void *va,
				pdir_t npe,
				int	flags,
				STPD	)
{
	physaddr_t ptpa, opa, cur_dir;
	pdir_t *pd;
	pdir_t *pt = 0;
	pdir_t	pe;
	int	pti, pdi;

	pti = PAGING_ADDR_PTIDX( va );
	pdi = PAGING_ADDR_PDIDX( va );

	assert( pdi != PAGING_RM_PDIDX );
	assert( pdi != PAGING_ST_PDIDX );

	cur_dir = paging_getdir();

	if ( cur_dir == dir )
		pd = PAGING_RM_P_PDIR;
	else {
		pd = paging_mkwindow( dir, STPC );

		if ( STPF )
			return 0;
	}

	pe = pd[ pdi ];

	if( ! ( pe & PAGING_BIT_PRESENT ) ) {

		ptpa = physmm_alloc_frame();

		if ( ptpa == PHYSMM_NO_FRAME ) {
		
			if ( dir != cur_dir )
				paging_rmwindow( pd, STPC );

			STERR( 0, STCODE_NOMEM, "No physical memory left to "
						"allocate page table" );
		}
		
		pe = 	PAGING_BIT_WRITE 	|
			PAGING_BIT_PRESENT	|
			PAGING_BIT_USER		|
			PAGING_PHYSADDR( ptpa );

		if ( flags & PAGE_FLAG_GLOBAL ) {
			pe |= PAGING_BIT_GLOBAL;
#ifdef OPT_heapmemory
			//TODO: Fill other page tables
#else
			pd[ pdi ] = pe;
#endif
		} else
			pd[ pdi ] = pe;

		if ( cur_dir == dir ) {

			pt = PAGING_RM_P_PTAB( pdi );
			i386_invtlb_addr( pt );
			
		} else {

			pt = paging_mkwindow( ptpa, STPC );
			
			if ( STPF ) {

				physmm_free_frame( ptpa );

				pd[ pdi ] = 0;

				if ( dir != cur_dir )
					paging_rmwindow( pd, STPC );

				STERR( 0, STCODE_NORES,
					"No physical memory window available" );
			}

		}
	
		memset( pt, 0, PAGING_PAGETBL_LENGTH * sizeof( pdir_t ) );

	} else if ( flags & PAGE_FLAG_GLOBAL ) {
		assert( pe & PAGING_BIT_GLOBAL );
	} else if ( ! ( ( flags & PAGE_FLAG_GLOBAL ) || 
			( flags & PAGE_FLAG_IMPL1 ) ) ) {
		assert( ! ( pe & PAGING_BIT_GLOBAL ) );
	}

	if ( !pt ) {

		if ( cur_dir == dir ) {

			pt = PAGING_RM_P_PTAB( pdi );
			
		} else {

			pt = paging_mkwindow( PAGING_PHYSADDR( pe ), STPC );
		
			if ( STPF ) {
			
				if ( dir != cur_dir )
					paging_rmwindow( pd, STPC );
				
				STERR( 0, STCODE_NORES,
					"No physical memory window available" );

			}

		}
	}

	pe = pt[ pti ];

	if ( pe & PAGING_BIT_PRESENT )
		opa = PAGING_PHYSADDR( pe );
	else
		opa = PHYSMM_NO_FRAME;

	if ( flags & PAGE_FLAG_IMPL1 ) {

		if ( npe == PAGE_PHYS_UNCHANGED )
			npe = pe & ~0xFFF;
		else
			npe = npe & ~0xFFF;

		npe = npe | ( pe & ( 0xFFF & ~PAGING_BIT_PRESENT ) );
	}

	pt[ pti ] = npe;

	if ( dir == cur_dir )
		i386_invtlb_addr( va );
	else {
		
		paging_rmwindow( pt, STPC );
		paging_rmwindow( pd, STPC );

	}

	return opa;

}

physaddr_t      paging_map(	physaddr_t dir,
				void *va,  
				physaddr_t pa,
				int	flags,
				STPD	)
{
	
	pdir_t pe;
	
	pe =	PAGING_BIT_PRESENT	|
		PAGING_PHYSADDR( pa );

	if ( flags & PAGE_FLAG_GLOBAL )
		pe |= PAGING_BIT_GLOBAL;

	if ( flags & PAGE_PERM_W )
		pe |= PAGING_BIT_WRITE;

	if ( flags & PAGE_PERM_U )
		pe |= PAGING_BIT_USER;

	return paging_setpe( dir, va, pe, flags, STPC );

}


physaddr_t      paging_swmap(	physaddr_t dir,
				void *va,  
				physaddr_t pa,
				STPD	)
{
	pdir_t pe;

	pe =  (physaddr_t) pa;

	return paging_setpe( dir, va, pe, PAGE_FLAG_IMPL1, STPC );
}

physaddr_t	paging_unmap(	physaddr_t dir,
				void *va,
				STPD	)
{
	return paging_setpe( dir, va, 0, PAGE_FLAG_IMPL1, STPC );
}


physaddr_t	paging_newdir(	physaddr_t	dir,
				STPD	)
{

	int		 i1, stc;
	char *		 ste;
	pdir_t		*spd;
	pdir_t		*dpd;
	pdir_t		 pde;
	physaddr_t	 cur_dir, nd;	

	if ( dir == 0 ) {
		
		nd = physmm_alloc_frame();

		if ( nd == PHYSMM_NO_FRAME ) {
			STERR(0, STCODE_NOMEM,
				 "No memory left trying to allocate page dir");
		}

		dpd = paging_mkwindow( nd, STPC );

		if ( STPF ) {
			STERR(0, STCODE_NORES,
				 "No physical memory window available" );
		}

		memset( dpd, 0, sizeof( pdir_t ) * PAGING_PAGEDIR_LENGTH );

		paging_rmwindow( dpd, STPC );
	
		return nd;

	}

	cur_dir = paging_getdir();

	if ( dir == cur_dir )
		spd = PAGING_RM_P_PDIR;
	else {

		spd = paging_mkwindow( dir, STPC );
		

		if ( STPF ) {
			ste = "No physical memory window available for source"
			      " dir";
			stc = STCODE_NORES;
			goto _bail_no_sdwnd;
		}
	}

	nd = physmm_alloc_frame();

	if ( nd == PHYSMM_NO_FRAME ) {
		
		stc = STCODE_NOMEM;
		ste = "No memory left trying to allocate new page directory";
		goto _bail_no_ndmem;
	}

	dpd = paging_mkwindow( dir, STPC );

	if ( STPF ) {
		
		stc = STCODE_NORES;
		ste = "No physical memory window available for dest dir";
		goto _bail_no_ndwnd;
	
	}

	for ( i1 = 0; i1 < PAGING_PAGEDIR_LENGTH; i1++ ) {

		pde = spd[ i1 ];
		
		if ( ! ( pde & PAGING_BIT_PRESENT ) ) {

			dpd[ i1 ] = 0;

			continue;
		}

		if ( ! (pde & PAGING_BIT_GLOBAL ) ) {
			dpd[ i1 ] = 0;

			continue;
		}

		dpd[ i1 ] = pde;

	}

	if ( dir != cur_dir )
		paging_rmwindow( spd, STPC );

	paging_rmwindow( dpd, STPC );

	return nd;

_bail_no_ndwnd:
	physmm_free_frame( nd );

_bail_no_ndmem:
	if ( dir != cur_dir )
		paging_rmwindow( spd, STPC );

_bail_no_sdwnd:
	STERR(0, stc, ste);

}

void	paging_freedir(		physaddr_t	dir,
				STPD	)
{

	int		 i1, i2, stc;
	char *		 ste;
	pdir_t		*pd;
	pdir_t		*pt;
	pdir_t		 pde;
	pdir_t		 pte;

	assert( dir != paging_getdir() );

	pd = paging_mkwindow( dir, STPC );

	if ( STPF ) {
		ste = "No physical memory window available for page directory";
		stc = STCODE_NORES;
		goto _bail_no_dwnd;
	}

	for ( i1 = 0; i1 < PAGING_PAGEDIR_LENGTH; i1++ ) {

		pde = pd[ i1 ];
		
		if ( ! ( pde & PAGING_BIT_PRESENT ) ) 
			continue;

		if ( pde & PAGING_BIT_GLOBAL ) 
			continue;

		pt = paging_mkwindow( PAGING_PHYSADDR( pde ), STPC );

		if ( STPF ) {
			ste =  	"No physicial memory window available for page "
				"table";
			stc = STCODE_NORES;

			goto _bail_no_twnd;
		}		

		for ( i2 = 0; i2 < PAGING_PAGETBL_LENGTH; i2++ ) {
			pte = pt[ i2 ];			

			if ( pte & PAGING_BIT_PRESENT ) {

				ste =	"Page directory still has frames "
					"attached";
				stc = STCODE_INUSE;
				goto _bail_inuse;

			}
		}
	
		paging_rmwindow( pt, STPC );

	}
	
	for ( i1 = 0; i1 < PAGING_PAGEDIR_LENGTH; i1++ ) {

		pde = pd[ i1 ];

		physmm_free_frame( PAGING_PHYSADDR( pde ) );

	}

	paging_rmwindow( pd, STPC );

	physmm_free_frame( PAGING_PHYSADDR( dir ) );

	return;

_bail_inuse:
	paging_rmwindow( pt, STPC);
_bail_no_twnd:
	paging_rmwindow( pd, STPC);
_bail_no_dwnd:
	STERRV(stc, ste);

}
