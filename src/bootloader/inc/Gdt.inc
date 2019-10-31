; -------------------------------------------------------------
; Clementine Bootloader:
;	Please refer to the LICENSE file you should have received
;	with this software.
;
;	Gdt.inc
;	  
;	-- Luke Smith	
; -------------------------------------------------------------

%ifndef __JUMP_GDT__
%define __JUMP_GDT__

[BITS 16]

InstallGDT:

	cli
	pusha
	lgdt 	[toc]
	sti	
	popa
	ret	

gdt_desc: 
; gdt null:
	dd 0
	dd 0 

; gdt kernel code:	
	dw 0FFFFh
	dw 0
	db 0
	db 10011010b		
	db 11001111b
	db 0

; gdt kernel data:	
	dw 0FFFFh
	dw 0
	db 0
	db 10010010b	
	db 11001111b
	db 0

end_of_gdt:
toc: 
	dw end_of_gdt - gdt_desc - 1 
	dd gdt_desc 		

; give the descriptor offsets names

%define NULL_DESC 0
%define KERN_CODE_DESC 08h
%define KERN_DATA_DESC 010h
%endif
