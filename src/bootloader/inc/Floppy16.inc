; -------------------------------------------------------------
; Clementine Bootloader:
;   Please refer to the LICENSE file you should have received
;   with this software.
;
;   Floppy16.inc
;	 
;	-- Luke Smith   
; -------------------------------------------------------------

%ifndef __JUMP_FLOPPY16__
%define __JUMP_FLOPPY16__

[BITS 16]

OEMLabel:			db  "MRDOS 1.0"
BytesPerSector:		dw	512
SectorsPerCluster:	db	1
ReservedForBoot:	dw	1
NumberOfFats:		db	2
RootDirEntries:		dw	224
LogicalSectors:		dw	2880
MediumByte:			db	0xf0
SectorsPerFat:		dw	9
SectorsPerTrack:	dw	18
HeadsPerCylinder:	dw	2
HiddenSectors:		dd	0
LargeSectors:		dd	0
DriveNo:			db	0
Unused:				db	0
Signature:			db	0x29
VolumeID:			dd	0xa0a1a2a3
VolumeLabel:		db	"CLEMENTINE "
FileSystem:			db	"FAT12   "

datasector	dw 0x0000
cluster		dw 0x0000

absoluteSector	db 0x00
absoluteHead	db 0x00
absoluteTrack	db 0x00

msgProgress	db ".", 0x00

ReadSectors:
	.MAIN:
		mov		di, 0x0005
	.SECTORLOOP:
		push	ax
		push	bx
		push	cx
		call	LBACHS
		mov		ah, 0x02
		mov		al, 0x01
		mov		ch, BYTE [absoluteTrack]
		mov		cl, BYTE [absoluteSector]
		mov		dh, BYTE [absoluteHead]
		mov		dl, BYTE [DriveNo]
		int		0x13
		jnc		.SUCCESS
		xor		ax, ax
		int		0x13
		dec		di
		pop		cx
		pop		bx
		pop		ax
		jnz		.SECTORLOOP
		int		0x18
	.SUCCESS:
		mov		si, msgProgress
		call	Puts16
		pop		cx
		pop		bx
		pop		ax
		add		bx, WORD [BytesPerSector]
		inc		ax
		loop	.MAIN
		ret

ClusterLBA:
		sub		ax, 0x0002
		xor		cx, cx
		mov		cl, BYTE [SectorsPerCluster]
		mul		cx
		add		ax, WORD [datasector]
		ret

LBACHS:
		xor		dx, dx
		div		WORD [SectorsPerTrack]
		inc		dl
		mov		BYTE [absoluteSector], dl
		xor		dx, dx
		div		WORD [HeadsPerCylinder]
		mov		BYTE [absoluteHead], dl
		mov		BYTE [absoluteTrack], al
		ret


%endif	; __JUMP_FLOPPY16__