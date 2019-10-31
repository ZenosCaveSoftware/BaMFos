; -------------------------------------------------------------
; Clementine Bootloader:
;	Please refer to the LICENSE file you should have received
;	with this software.
;
;	stdio.inc
;	  
;	-- Luke Smith	
; -------------------------------------------------------------

%ifndef __JUMP_STDIO__
%define __JUMP_STDIO__

bits 16

Puts16:
		pusha
.Loop1:
		lodsb
		or	al, al
		jz	Puts16Done
		mov	ah, 0x0E
		int	0x10
		jmp	.Loop1
Puts16Done:
		popa
		ret


bits 32

%define VIDMEM	0xB8000
%define COLS	80
%define LINES	25
%define CHAR_ATTR 0x0f
%define BLANK 0x20

_CurX db 0
_CurY db 0

Putch32:
	pusha
	mov edi, VIDMEM
	xor eax, eax

	mov ecx, COLS*2
	mov al, byte [_CurY]
	mul ecx
	push eax

	mov al, byte [_CurX]
	mov cl, 2
	mul cl
	pop ecx
	add eax, ecx

	xor ecx, ecx
	add edi, eax

	cmp bl, 0x0A
	je .Row

	mov dl, bl
	mov dh, CHAR_ATTR
	mov word [edi], dx

	inc byte [_CurX]
	;cmp byte [_CurX], COLS
	;je .Row
	jmp .done

.Row:
	mov byte [_CurX], 0
	inc byte [_CurY]
	;cmp byte [_CurY], LINES
	;je .Scroll

.done:
	popa
	ret

.Scroll:
	mov ecx, COLS * (LINES - 1)
	mov eax, 0
.L0:
	mov dx,	word [eax + VIDMEM + COLS]
	mov word[eax + VIDMEM], dx
	inc eax
	dec ecx
	jne .L0
	mov eax, COLS
.L1:
	mov dl, BLANK
	mov dh, CHAR_ATTR
	mov word [eax + VIDMEM], dx
	inc eax
	dec ecx
	jne .L1
	xor eax, eax
	jmp .done

Puts32:
	pusha
	push ebx
	pop edi

.loop:
	mov bl, byte [edi]
	cmp bl, 0
	je .done

	call Putch32

	inc edi
	jmp .loop
.done:
	
	mov bh, byte [_CurY]
	mov bl, byte [_CurX]
	call MovCur

	popa
	ret

bits 32

MovCur:
	
	pusha

	xor eax, eax
	mov ecx, COLS
	mov al, bh
	mul ecx
	add al, bl
	mov ebx, eax

	mov al, 0x0F
	mov dx, 0x03D4
	out dx, al

	mov al, bl
	mov dx, 0x03D5
	out dx, al

	xor eax, eax

	mov al, 0x0E
	mov dx, 0x03D4
	out dx, al

	mov al, bh
	mov dx, 0x03D5
	out dx, al

	popa 
	ret

bits 32

ClrScr32:

	pusha
	cld
	mov edi, VIDMEM
	mov cx, 2000
	mov ah, CHAR_ATTR
	mov al, ' '
	rep stosw

	mov byte [_CurX], 0
	mov byte [_CurY], 0
	popa
	ret

bits 32

GotoXY:
	pusha
	mov [_CurX], al
	mov [_CurY], ah
	popa
	ret
	

%endif ;__JUMP_STDIO__
