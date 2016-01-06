;Copyright (C) 2015,2016 Peter Bosch
;
;This program is free software; you can redistribute it and/or
;modify it under the terms of the GNU General Public License
;as published by the Free Software Foundation; either version 2
;of the License, or (at your option) any later version.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program; if not, write to the Free Software
;Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

;
; @file src/archlib/i386/tlb.s
;
; Part of posnk kernel
;
; Written by Peter Bosch <peterbosc@gmail.com>
;

[BITS 32]
[section .text]

; Invalidate TLB for address
; void i386_invtlb_addr( void *addr );
[global i386_invtlb_addr]
i386_invtlb_addr:
	mov	eax,	[esp + 4]	; Move first parameter into eax
	invlpg	[eax]			; Invalidate TLB for addr in eax
	ret				; Return

; Globally invalidate TLB
; void i386_invtlb_all( void );
[global i386_invtlb_all]
i386_invtlb_all:
	mov	eax,	cr3		; Reload cr3 with its value
	mov	cr3,	eax
	ret

; vim: syntax=nasm
