#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <io/vga.h>
#include <io/tty.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/tss.h>
#include <boot/multiboot.h>
#include <mem/paging.h>
#include <mem/mem.h>
#include <mem/alloc.h>

void kernel_early(multiboot_t *mboot, uint32_t magic, uintptr_t esp)
{
	initialize_terminal();
	terminal_writestring("[TTY]   ...  \x1b[32mDONE\n\x1b[00m[GDT]   ... ");
	initialize_gdt();
	terminal_writestring("\x1b[32mDONE\n\x1b[00m[IDT]   ... ");
	initialize_idt();
	terminal_writestring("\x1b[32mDONE\n\x1b[00m[IRQ]   ... ");
	initialize_irq();
	__asm__ __volatile__("sti");
	terminal_writestring("\x1b[32mDONE\n\x1b[00m[PAGE]  ... ");
	initialize_paging(mboot->mem_upper + mboot->mem_lower);
	terminal_writestring("\x1b[32mDONE\n\x1b[00m");

}

void kernel_main()
{
	printf("\
 _____       __  __  _____    _____ _____      \n\
 | ___ \\     |  \\/  ||  ___|  /  _  /  ___|   \n\
 | |_/ / __ _| .  . || |_ ____| | | \\ `--.     \n\
 | ___ \\/ _` | |\\/| ||  _|____| | | |`--. \\  \n\
 | |_/ / (_| | |  | || |      \\ \\_/ /\\__/ /  \n\
 \\____/ \\__,_\\_|  |_/\\_|       \\___/\\____/\n\
 Barely		  Made	  Functional: An OS.\n");
}