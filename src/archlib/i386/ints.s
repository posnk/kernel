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
; @file src/archlib/i386/ints.s
;
; Part of posnk kernel
;
; Written by Peter Bosch <peterbosc@gmail.com>
;

[BITS 32]
[section .text]

; Disable interrupts
; void i386_cli( void );
[global i386_cli]
i386_cli:
	cli				; Disable interrupts
	ret				; Return

; Enable interrupts
; void i386_sti( void );
[global i386_sti]
i386_sti:
	sti				; Enable interrupts
	ret				; Return


; vim: syntax=nasm
