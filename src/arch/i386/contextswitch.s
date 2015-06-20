[BITS 32]
[section .text]


; uint32_t i386_get_return_address( void );
; returnx 0xFFFDCAFE when a switch completed, and the current IP at the call if
; we are preparing a switch
[global i386_get_return_address]
i386_get_return_address:
  pop eax
  jmp eax 

; vnoret i386_do_context_switch(	uint32_t esp, 
									uint32_t ebp, 
									uint32_t eip, 
									uint32_t pdir
								);
 
[global i386_do_context_switch]
i386_do_context_switch:
	mov ebp, esp
	mov eax, [ebp+4]  ; param 1 (esp)
	mov ebx, [ebp+8]  ; param 2 (ebp)
	mov ecx, [ebp+12] ; param 3 (eip)
	mov edx, [ebp+16] ; param 4 (page directory phys_addr)

	mov esp, eax
	mov ebp, ebx
	mov cr3, edx
	mov eax, 0xFFFDCAFE

	jmp ecx
	
	
[extern dbgapi_invoke_kdbg]
[global i386_cs_debug_attach]
i386_cs_debug_attach:
	mov ebp, esp
	cli
	mov eax, [ebp+4]  ; param 1 (esp)
	mov ebx, [ebp+8]  ; param 2 (ebp)
	mov ecx, [ebp+12] ; param 3 (eip)
	mov edx, [ebp+16] ; param 4 (page directory phys_addr)

	mov esp, eax
	mov ebp, ebx
	mov cr3, edx
	mov eax, 0xFFFDCAFE
	push ecx
	push ebx
	mov ebp, esp
	push ecx
	call dbgapi_invoke_kdbg
	hlt
