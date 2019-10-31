
; -------------------------------------------------------------
; Clementine Bootloader:
;   Please refer to the LICENSE file you should have received
;   with this software.
;
;   Fat12.inc
;
;	-- Luke Smith
; -------------------------------------------------------------

%ifndef __JUMP_FAT12__
%define __JUMP_FAT12__

[BITS 16]

%include "Floppy16.inc"

%define ROOT_OFFSET 0x2e00
%define FAT_SEG 0x2c0
%define ROOT_SEG 0x2e0

msgFound db	"Found file!", 0x0D, 0x0A, 0x00
msgRead db "Read File!", 0x0D, 0x0A, 0x00
LoadRoot:
	pusha
	push es

	xor		cx, cx
	xor		dx, dx
	mov		ax, 32
	mul		WORD [RootDirEntries]
	div		WORD [BytesPerSector]
	xchg	ax, cx

	mov		al, BYTE [NumberOfFats]
	mul		WORD [SectorsPerFat]
	add		ax, WORD [ReservedForBoot]
	mov		WORD [datasector], ax
	add		WORD [datasector], cx

	push	word ROOT_SEG
	pop		es
	mov		bx, 0
	call	ReadSectors
	pop		es
	popa
	ret

LoadFat:
	pusha
	push	es

	xor		ax, ax
	mov		al, BYTE [NumberOfFats]
	mul		WORD [SectorsPerFat]
	mov		cx, ax

	mov		ax, WORD [ReservedForBoot]

	push	word FAT_SEG
	pop		es
	xor		bx, bx
	call	ReadSectors
	pop		es
	popa
	ret

FindFile:
	push	cx
	push	dx
	push	bx
	mov		bx, si

	mov 	cx, WORD [RootDirEntries]
	mov		di, ROOT_OFFSET
	cld

.LOOP:
	push	cx
	mov		cx, 0x0B
	mov		si, bx
	push	di
rep cmpsb
	pop 	di
	je		.Found
	pop		cx
	add 	di, 32
	loop 	.LOOP

.NotFound:
	pop		bx
	pop		dx
	pop		cx
	mov		ax, -1
	ret

.Found:
	mov 	si, msgFound
	call 	Puts16
	pop		ax
	pop		bx
	pop		dx
	pop		cx
	ret


LoadFile:
	
	xor		ecx, ecx
	push	ecx

.FIND_FILE:
	
	push	bx
	push	bp
	call	FindFile
	cmp		ax, -1
	jne		.LOAD_IMAGE_PRE
	pop		bp
	pop		bx
	pop		ecx
	mov		ax, -1
	ret

.LOAD_IMAGE_PRE:
	sub		edi, ROOT_OFFSET
	sub		eax, ROOT_OFFSET

	push 	word ROOT_SEG
	pop		es
	mov		dx, WORD [es:di + 0x001A]
	mov		WORD [cluster], dx
	pop		bx
	pop		es
	push	bx
	push	es
	call	LoadFat

.LOAD_IMAGE:
	mov		ax, WORD [cluster]
	pop		es
	pop		bx
	call	ClusterLBA
	xor		cx, cx
	mov		cl, BYTE [SectorsPerCluster]
	call	ReadSectors
	mov 	si, msgRead
	call	Puts16
	pop		ecx
	inc		ecx
	push	ecx
	push	bx
	push	es
	mov		ax, FAT_SEG
	mov		es, ax
	xor		bx, bx

	mov		ax, WORD [cluster]
	mov		cx, ax
	mov		dx, ax
	shr		dx, 0x0001
	add		cx, dx

	mov		bx, 0
	add		bx, cx
	mov		dx, WORD[es:bx]
	test	ax, 0x0001
	jnz		.ODD_CLUSTER

.EVEN_CLUSTER:
	and		dx, 0000111111111111b
	jmp		.DONE

.ODD_CLUSTER:
	shr		dx, 0x0004

.DONE:
	mov		WORD [cluster], dx
	cmp		dx, 0x0ff0
	jb		.LOAD_IMAGE

.SUCCESS:
	pop		es
	pop		bx
	pop		ecx
	xor		ax,ax
	ret

%endif		; __JUMP_FAT12__