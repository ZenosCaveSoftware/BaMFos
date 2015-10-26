#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/tss.h>
#include <kernel/multiboot.h>
#include <kernel/paging.h>

void kernel_early(multiboot_t *mboot, uint32_t magic, uintptr_t esp)
{

	initialize_terminal();
	terminal_writestring("[GDT]   ... ");
	initialize_gdt();
	terminal_writestring("DONE\n[IDT]   ... ");
	initialize_idt();
	terminal_writestring("DONE\n[IRQ]   ... ");
	initialize_irq();
	terminal_writestring("DONE\n[PAGE]  ... ");
	__asm__ __volatile__("sti");
	initialize_paging(mboot->mem_upper + mboot->mem_lower);
	terminal_writestring("DONE\n");
}

void kernel_main()
{
printf("\
 ______      ___  _________    _____ _____      \n\
 | ___ \\     |  \\/  ||  ___|  /  _  /  ___|   \n\
 | |_/ / __ _| .  . || |_ ____| | | \\ `--.     \n\
 | ___ \\/ _` | |\\/| ||  _|____| | | |`--. \\  \n\
 | |_/ / (_| | |  | || |      \\ \\_/ /\\__/ /  \n\
 \\____/ \\__,_\\_|  |_/\\_|       \\___/\\____/\n\
 Barely		  Made	  Functional: An OS.\n\
 ");
}
