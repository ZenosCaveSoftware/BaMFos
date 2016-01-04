#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <cpu/gdt.h>
#include <cpu/tss.h>
#include <misc/utils.h>
#include <io/tty.h>

static struct {
	gdt_entry_t entries[6];
	gdt_ptr_t 	pointer;
	tss_entry_t tss;
} gdt __attribute__((used));

void initialize_gdt()
{
	gdt_ptr_t *gdt_ptr = &gdt.pointer;
	gdt_ptr->limit = sizeof gdt.entries - 1;
	gdt_ptr->base = (uintptr_t)&gdt.entries[0];
	
	fillgdte(0, (gdt_t){.base=0, .limit=0, .type=0}, false);			 // Null segment
	fillgdte(1, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0x9A}, false); // Code segment
	fillgdte(2, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0x92}, false); // Data segment
	fillgdte(3, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0xFA}, false); // User mode code segment
	fillgdte(4, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0xF2}, false); // User mode data segment

	write_tss(5, 0x10, 0x0);
	gdt_flush((uintptr_t)gdt_ptr, 0x8, 0x10);
    //__asm__ __volatile__ ("ltr %%ax": : "a" (5<<3));
	tss_flush();
}

static void fillgdte(int32_t num, gdt_t source, bool is_tss)
{
	
	if (source.limit > 65536)
	{
		source.limit = source.limit >> 12;
		gdt.entries[num].granularity = 0xC0;
	}
	else
	{
		gdt.entries[num].granularity = 0x40;
	}

	if(is_tss)
	{
		gdt.entries[num].granularity = 0x00;
	}

	//Encode Limit
	gdt.entries[num].limit_low = source.limit & 0xFFFF;
	gdt.entries[num].granularity |= (source.limit >> 16) & 0x0F;
	
	//Encode Base
	gdt.entries[num].base_low = source.base & 0xFFFF;
	gdt.entries[num].base_middle = (source.base >> 16) & 0xFF;
	gdt.entries[num].base_high   = (source.base >> 24) & 0xFF;

	//Encode Access Type
	gdt.entries[num].access = source.type;
}

static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0)
{

	tss_entry_t * tss = &gdt.tss;
	uintptr_t base = (uintptr_t)tss;
	uintptr_t limit = base + sizeof *tss;

	fillgdte(num, (gdt_t){.base=base, .limit=limit, .type=0xE9}, true);

	memset((void *)tss, 0x0, sizeof *tss);

	tss->ss0 = ss0;
	tss->esp0 = esp0;
	tss->cs = 0x0b;
	tss->ss = 0x13;
	tss->ds = 0x13;
	tss->es = 0x13;
	tss->fs = 0x13;
	tss->gs = 0x13;

	tss->iomap_base = sizeof *tss;
}
