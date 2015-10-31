# Not Working yet...
.global _start
.extern kernel_early
.extern kernel_main

.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)

.section .data
.align 0x1000
BootPageDirectory:
	.long 0x00000083
	times (KERNEL_PAGE_NUMBER - 1) .long 0
	.long 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) .long 0

.section .text
.align 4
MultiBootHeader:
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.set STACKSIZE, 0x4000
.set loader, (_start - 0xC0000000)
.global loader

_start:
	mov (BootPageDirectory - KERNEL_VIRTUAL_BASE), %ecx
	mov %ecx, %cr3


	mov %cr4, %ecx
	or 	0x00000010, %ecx
	mov %ecx, %cr4

	mov %cr0, %ecx
    or 0x80000000, %ecx
    mov %ecx, %cr0

    lea  [StartinHigherHalf], %ecx
    jmp %ecx

StartinHigherHalf:
	mov dword 0, [BootPageDirectory]
	invlpg [0]

	mov stack+STACKSIZE, %esp
	push %eax
	push %ebx

	cli

	call kernel_early

	call _init

	call kernel_main

.Lhang:
	hlt
	jmp.Lhang

.section .bss
.align 32
stack:
	resb STACKSIZE
