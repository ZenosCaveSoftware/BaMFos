; -------------------------------------------------------------
; Clementine Bootloader:
;	Please refer to the LICENSE file you should have received
;	with this software.
;
;	Stage1.asm
;	  
;	-- Luke Smith	
; -------------------------------------------------------------

org 0h
bits 16

jmp main

%include "Floppy16.inc"

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


Print:
		lodsb
		or	al, al
		jz	PrintDone
		mov	ah, 0x0E
		int	0x10
		jmp	Print
	PrintDone:
		ret


main:
		cli
		mov	ax, 0x07C0
		mov	ds, ax
		mov	es, ax
		mov	fs, ax
		mov	gs, ax

		mov	ax, 0x0000
		mov	ss, ax
		mov	sp, 0xFFFF
		sti
	
	LOAD_ROOT:
		xor	cx, cx
		xor	dx, dx
		mov	ax, 0x0020
		mul	WORD [RootDirEntries]
		div	WORD [BytesPerSector]
		xchg	ax, cx

		mov	al, BYTE [NumberOfFats]
		mul	WORD [SectorsPerFat]
		add	ax, WORD [ReservedForBoot]
		mov	WORD [datasector], ax
		add	WORD [datasector], cx

		mov	bx, 0x0200
		call	ReadSectors

		mov	cx, WORD [RootDirEntries]
		mov	di, 0x0200
	.LOOP:
		push	cx
		mov	cx, 0x0B
		mov	si, ImageName
		push	di
		repe	cmpsb
		pop	di
		je	LOAD_FAT
		pop	cx
		add	di, 0x0020
		loop	.LOOP
		jmp	FAILURE

	LOAD_FAT:
		mov	si, msgCRLF
		call	Print
		mov	dx, WORD [di + 0x001A]
		mov	WORD [cluster], dx

		xor	ax, ax
		mov	al, BYTE [NumberOfFats]
		mul	WORD [SectorsPerFat]
		mov	cx, ax

		mov	ax, WORD [ReservedForBoot]

		mov	bx, 0x0200
		call	ReadSectors

		mov	si, msgCRLF
		call	Print
		mov	ax, 0x0050
		mov	es, ax
		mov	bx, 0x0000
		push	bx

	LOAD_IMAGE:
		mov	ax, WORD [cluster]
		pop	bx
		call	ClusterLBA
		xor	cx, cx
		mov	cl, BYTE [SectorsPerCluster]
		call	ReadSectors
		push	bx

		mov	ax, WORD [cluster]
		mov	cx, ax
		mov	dx, ax
		shr	dx, 0x0001
		add	cx, dx
		mov	bx, 0x0200
		add	bx, cx
		mov	dx, WORD [bx]
		test	ax, 0x0001
		jnz	.ODD_CLUSTER

	.EVEN_CLUSTER:
	
		and	dx, 0000111111111111b
		jmp	.DONE

	.ODD_CLUSTER:
	
		shr	dx, 0x0004

	.DONE:

		mov	WORD [cluster], dx
		cmp	dx, 0x0FF0
		jb	LOAD_IMAGE

	DONE:
	
		mov	si, msgCRLF
		call	Print
		push	WORD 0x0050
		push	WORD 0x0000
		retf

	FAILURE:
	
		mov	si, msgFailure
		call	Print
		mov	ah, 0x00
		int	0x16
		int	0x19

	ImageName	db "KRNLDR  SYS"
	msgCRLF		db 0x0D, 0x0A, 0x00
	msgFailure	db 0x0D, 0x0A, "ERROR on boot", 0x0D, 0x0A, 0x00

		times	510-($-$$) db 0
		dw		0xAA55
