#include <stdint.h>
#include <string.h>
#include <kernel/dts.h>
#include <kernel/utils.h>
#include <kernel/tty.h>

gdt_entry_t gdt[5];
gdt_ptr_t	gdt_ptr;
idt_entry_t idt[256];
idt_ptr_t	idt_ptr;
tss_entry_t tss;

void initialize_descriptor_tables()
{
	terminal_writestring("Initializing Descriptor Tables...\n-- Initializing GDT...\t");
	initialize_gdt();
	terminal_writestring("[DONE]\n-- Initializing IDT...\n");
	initialize_idt();
	terminal_writestring("[DONE]\n-- Initializing irq...\t");
	initialize_irq();
	terminal_writestring("[DONE]\n[DONE]\n");	
	__asm__ __volatile__("sti");
}


static void initialize_gdt()
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base  = (uint32_t)&gdt;
	
	fillgdte(gdt+0, (gdt_t){.base=0, .limit=0, .type=0}, false);			 // Null segment
	fillgdte(gdt+1, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0x9A}, false); // Code segment
	fillgdte(gdt+2, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0x92}, false); // Data segment
	fillgdte(gdt+3, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0xFA}, false); // User mode code segment
	fillgdte(gdt+4, (gdt_t){.base=0, .limit=0xFFFFFFFF, .type=0xF2}, false); // User mode data segment

	terminal_writestring("\n-- Writing TSS...\t");
	write_tss(gdt+5, 0x10, 0x0);

	gdt_flush((uint32_t)&gdt_ptr);
	terminal_writestring("[DONE]\n\t-- Flushed GDT\n\t--Flushig TSS...\t\t");
    //__asm__ __volatile__ ("ltr %%ax": : "a" (5<<3));
	tss_flush();
	terminal_writestring("[DONE]\n");
}

void initialize_idt()
{
	idt_ptr.limit = (sizeof (idt_entry_t) * 256) - 1;
	idt_ptr.base = (uint32_t)&idt;
	memset((void *)idt_ptr.base, 0, sizeof (idt_entry_t) * 256);	

	fillidte(idt+0, 8, int0handler(), 0xe, 0);//exception handlers
	fillidte(idt+1, 8, int1handler(), 0xe, 0);
	fillidte(idt+2, 8, int2handler(), 0xe, 0);
	fillidte(idt+3, 8, int3handler(), 0xe, 0);
	fillidte(idt+4, 8, int4handler(), 0xe, 0);
	fillidte(idt+5, 8, int5handler(), 0xe, 0);
	fillidte(idt+6, 8, int6handler(), 0xe, 0);
	fillidte(idt+7, 8, int7handler(), 0xe, 0);
	fillidte(idt+8, 8, int8handler(), 0xe, 0);
	fillidte(idt+9, 8, int9handler(), 0xe, 0);
	fillidte(idt+10, 8, int10handler(), 0xe, 0);
	fillidte(idt+11, 8, int11handler(), 0xe, 0);
	fillidte(idt+12, 8, int12handler(), 0xe, 0);
	fillidte(idt+13, 8, int13handler(), 0xe, 0);
	fillidte(idt+14, 8, int14handler(), 0xe, 0);
	fillidte(idt+16, 8, int16handler(), 0xe, 0);

	fillidte(idt+0x80, 8, int0x80handler(), 0xe, 3); //system call handler	

	idt_flush((uint32_t)&idt_ptr);
}

void initialize_irq()
{
	pic_remap(0x20, 0x28);

	fillidte(idt + 32, 8, irq0handler(), 0xe, 0);//IRQ handlers;
	fillidte(idt + 33, 8, irq1handler(), 0xe, 0);
	fillidte(idt + 34, 8, irq2handler(), 0xe, 0);
	fillidte(idt + 35, 8, irq3handler(), 0xe, 0);
	fillidte(idt + 36, 8, irq4handler(), 0xe, 0);
	fillidte(idt + 37, 8, irq5handler(), 0xe, 0);
	fillidte(idt + 38, 8, irq6handler(), 0xe, 0);
	fillidte(idt + 39, 8, irq7handler(), 0xe, 0);
	fillidte(idt + 40, 8, irq8handler(), 0xe, 0);
	fillidte(idt + 41, 8, irq9handler(), 0xe, 0);
	fillidte(idt + 42, 8, irq10handler(), 0xe, 0);
	fillidte(idt + 43, 8, irq11handler(), 0xe, 0);
	fillidte(idt + 44, 8, irq12handler(), 0xe, 0);
	fillidte(idt + 45, 8, irq13handler(), 0xe, 0);
	fillidte(idt + 46, 8, irq14handler(), 0xe, 0);
	fillidte(idt + 47, 8, irq15handler(), 0xe, 0);
}

void pic_remap(uint8_t offset1, uint8_t offset2)
{
	uint8_t	a1, a2;

	a1 = inb(PIC1_DATA);						// save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);	// start init seq
	io_wait();									// in cascade mode
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);	// ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);	// ICW2: sLAVE PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);		// ICW3: tell Master PIC that
	io_wait();				// there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);		// ICW3: tell Slave PIC
	io_wait();				// its cascade identity (0000 0010)
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
	outb(PIC1_DATA, a1);   						// restore
	outb(PIC2_DATA, a2);						// saved masks.
}

// Set the value of a GDT entry
static void fillgdte(gdt_entry_t* entry, gdt_t source, bool is_tss)
{
	if ((source.limit > 65536) && (source.limit & 0xFFF) != 0xFFF)
	{
		return;//TODO: implement KERROR.
	}

	if (source.limit > 65536)
	{
		source.limit = source.limit >> 12;
		entry->granularity = 0xC0;
	}
	else
	{
		entry->granularity = 0x40;
	}

	if(is_tss)
	{
		entry->granularity = 0x40;
	}
	//Encode Limit
	entry->limit_low = source.limit & 0xFFFF;
	entry->granularity |= (source.limit >> 16) & 0x0F;
	
	//Encode Base
	entry->base_low = source.base & 0xFFFF;
	entry->base_middle = (source.base >> 16) & 0xFF;
	entry->base_high   = (source.base >> 24) & 0xFF;

	//Encode Access Type
	entry->access = source.type;
}

static void write_tss(gdt_entry_t *gdtp, uint16_t ss0, uint32_t esp0)
{
	tss_entry_t * tssp = &tss;
	uintptr_t base = (uintptr_t)tssp;
	uintptr_t limit = base + sizeof *tssp;

	fillgdte(gdtp, (gdt_t){.base=base, .limit=limit, .type=0x89}, true);

	memset((void *)tssp, 0x0, sizeof *tssp);

	tssp->ss0 = ss0;
	tssp->esp0 = esp0;
	tssp->cs = 0x0b;
	tssp->ss = 0x13;
	tssp->ds = 0x13;
	tssp->es = 0x13;
	tssp->fs = 0x13;
	tssp->gs = 0x13;

	tssp->iomap_base = sizeof *tssp;
}

static void fillidte(idt_entry_t* entry, uint16_t sel, void *offset, uint8_t gate, uint8_t priv)
{
	entry->base_low =  (uint16_t)((uint32_t)offset & 0xFFFF);
	entry->base_high =  (uint16_t)(((uint32_t)offset >> 16) & 0xFFFF);

	entry->selector = sel,
	entry->zero = 0;

	entry->flags = 0x80 | ((priv & 0x03) << 5) | gate;
}