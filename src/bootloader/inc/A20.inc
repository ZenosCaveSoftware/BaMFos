; -------------------------------------------------------------
; Clementine Bootloader:
;   Please refer to the LICENSE file you should have received
;   with this software.
;
;   A20.inc
;	 
;	-- Luke Smith   
; -------------------------------------------------------------

%ifndef __JUMP_A20__
%define __JUMP_A20__

bits	16

;----------------------------------------------
; Enables a20 line through keyboard controller
;----------------------------------------------

EnableA20_Kbrd:

	cli
	push ax
	mov al,0xDD 
	out 0x64, al
	pop ax
	ret

;--------------------------------------------
; Enables a20 line through output port
;--------------------------------------------

EnableA20_Kbrd_Out:

	cli
	pusha
	call wait_input
	mov  al,0xAD
	out 0x64,al 
	call wait_input

	mov  al,0xD0
	out 0x64,al 
	call wait_output

	in   al,0x60
	push eax 
	call wait_input

	mov  al,0xD1
	out 0x64,al 
	call wait_input

	pop  eax
	or  al,2 
	out 0x60,al 

	call wait_input
	mov al,0xAE 
	out  0x64,al

	call wait_input
	popa
	sti
	ret



wait_input:
	in al,0x64
	test al,2
	jnz wait_input
	ret



wait_output:
	in al,0x64
	test al,1
	jz   wait_output
	ret

;--------------------------------------
; Enables a20 line through bios
;--------------------------------------

EnableA20_Bios:
	pusha
	mov ax, 0x2401
	int 0x15
	popa
	ret

;-------------------------------------------------
; Enables a20 line through system control port A
;-------------------------------------------------

EnableA20_SysControlA:
	push ax
	mov al, 2
	out 0x92, al
	pop ax
	ret

%endif
