#include <stdint.h>
#include <string.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <misc/utils.h>
#include <io/tty.h>

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();
extern void _isr127();

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

idt_entry_t idt[256];
idt_ptr_t 	idt_ptr;
void initialize_idt()
{
	idt_ptr.limit = (sizeof (idt_entry_t) * 256) - 1;
	idt_ptr.base = (uintptr_t)&idt;
	memset((void *)idt_ptr.base, 0, sizeof (idt_entry_t) * 256);	

	fillidte(0, 8, _isr0, 0xe, 0);
	fillidte(1, 8, _isr1, 0xe, 0);
	fillidte(2, 8, _isr2, 0xe, 0);
	fillidte(3, 8, _isr3, 0xe, 0);
	fillidte(4, 8, _isr4, 0xe, 0);
	fillidte(5, 8, _isr5, 0xe, 0);
	fillidte(6, 8, _isr6, 0xe, 0);
	fillidte(7, 8, _isr7, 0xe, 0);
	fillidte(8, 8, _isr8, 0xe, 0);
	fillidte(9, 8, _isr9, 0xe, 0);
	fillidte(10, 8, _isr10, 0xe, 0);
	fillidte(11, 8, _isr11, 0xe, 0);
	fillidte(12, 8, _isr12, 0xe, 0);
	fillidte(13, 8, _isr13, 0xe, 0);
	fillidte(14, 8, _isr14, 0xe, 0);
	fillidte(15, 8, _isr15, 0xe, 0);
	fillidte(16, 8, _isr16, 0xe, 0);
	fillidte(17, 8, _isr17, 0xe, 0);
	fillidte(18, 8, _isr18, 0xe, 0);
	fillidte(19, 8, _isr19, 0xe, 0);
	fillidte(20, 8, _isr20, 0xe, 0);
	fillidte(21, 8, _isr21, 0xe, 0);
	fillidte(22, 8, _isr22, 0xe, 0);
	fillidte(23, 8, _isr23, 0xe, 0);
	fillidte(24, 8, _isr24, 0xe, 0);
	fillidte(25, 8, _isr25, 0xe, 0);
	fillidte(26, 8, _isr26, 0xe, 0);
	fillidte(27, 8, _isr27, 0xe, 0);
	fillidte(28, 8, _isr28, 0xe, 0);
	fillidte(29, 8, _isr29, 0xe, 0);
	fillidte(30, 8, _isr30, 0xe, 0);
	fillidte(31, 8, _isr31, 0xe, 0);
	fillidte(127, 8, _isr127, 0xe, 0);

	idt_flush((uintptr_t)&idt_ptr);
}


void initialize_irq()
{
	pic_remap(0x20, 0x28);
	fillidte(32, 8, _irq0, 0xe, 0);
	fillidte(33, 8, _irq1, 0xe, 0);
	fillidte(34, 8, _irq2, 0xe, 0);
	fillidte(35, 8, _irq3, 0xe, 0);
	fillidte(36, 8, _irq4, 0xe, 0);
	fillidte(37, 8, _irq5, 0xe, 0);
	fillidte(38, 8, _irq6, 0xe, 0);
	fillidte(39, 8, _irq7, 0xe, 0);
	fillidte(40, 8, _irq8, 0xe, 0);
	fillidte(41, 8, _irq9, 0xe, 0);
	fillidte(42, 8, _irq10, 0xe, 0);
	fillidte(43, 8, _irq11, 0xe, 0);
	fillidte(44, 8, _irq12, 0xe, 0);
	fillidte(45, 8, _irq13, 0xe, 0);
	fillidte(46, 8, _irq14, 0xe, 0);
	fillidte(47, 8, _irq15, 0xe, 0);
}

static void fillidte(int entry, uint16_t sel, void *offset, uint8_t gate, uint8_t priv)
{
	idt[entry].base_low  =  (uint16_t)((uintptr_t)offset & 0xFFFF);
	idt[entry].base_mid  =  (uint16_t)(((uintptr_t)offset >> 16) & 0xFFFF);
	idt[entry].base_high =	(uint32_t)(((uintptr_t)offset >> 32) & 0xFFFFFFFF);

	idt[entry].selector = sel,
	idt[entry].reserved0 = 0;
	idt[entry].reserved1 = 0;

	idt[entry].flags = 0x80 | ((priv & 0x03) << 5) | gate;
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
