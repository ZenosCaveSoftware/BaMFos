#include <stdint.h>
#include <string.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/utils.h>
#include <kernel/tty.h>

idt_entry_t idt[256];
idt_ptr_t 	idt_ptr;
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

static void fillidte(idt_entry_t* entry, uint16_t sel, void *offset, uint8_t gate, uint8_t priv)
{
	entry->base_low =  (uint16_t)((uint32_t)offset & 0xFFFF);
	entry->base_high =  (uint16_t)(((uint32_t)offset >> 16) & 0xFFFF);

	entry->selector = sel,
	entry->zero = 0;

	entry->flags = 0x80 | ((priv & 0x03) << 5) | gate;
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
