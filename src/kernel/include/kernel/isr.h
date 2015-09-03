#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <stddef.h>

#define PIC1			0x20	/* IO base address for master PIC */
#define PIC2			0xA0	/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA		(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2+1)
#define PIC_READ_IRR	0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR	0x0b    /* OCW3 irq service next CMD read */

#define PIC_EOI			0x20	/* End-of-interrupt command code */

#define ICW1_ICW4		0x01	/* ICW4 (not) needed */
#define ICW1_SINGLE		0x02	/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04	/* Call address interval 4 (8) */
#define ICW1_LEVEL		0x08	/* Level Triggered (edge) mode */
#define ICW1_INIT		0x10	/* Initialization - required! */


#define ICW4_8086		0x01	/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO		0x02	/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define ICW4_SFNM		0x10	/* Special fully nested (not) */

void irqfunc(uint32_t irqnum, void *ctx);
void intfunc(uint32_t irqnum, void *ctx);
void intfunc_err(uint32_t irqnum, void *ctx, uint32_t errcode);

uint8_t pic_isnormalIRQ(uint32_t irqnum);
void pic_EOI(uint32_t irqnum);
void pic_EOI_spurious(uint32_t irqnum);

static uint16_t __pic_get_irq_reg(int ocw3);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);


/* Macro to create hardware interrupt handling functions. */
/* It will call "irqfunc" with the register context and the IRQ number as parameters. */
#define DEFIRQWRAPPER(irqnum)\
void *irq##irqnum##handler(void)\
{\
	volatile void *addr;\
	__asm__ goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	__asm__ __volatile__(".align 16" ::: "memory");\
	startofISR:\
	__asm__ __volatile__("pushal\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n\tcld" ::: "memory");\
	__asm__ __volatile__(\
		"pushl %%ds       \n\t"\
		"pushl %%es       \n\t"\
		"movw $16, %%cx   \n\t"\
		"movw %%cx, %%ds  \n\t"\
		"movw %%cx, %%es  \n\t"\
		"pushl %%ebp      \n\t"\
		"addl $4, (%%esp) \n\t"\
		"pushl %%ebx      \n\t"\
		"call *%%eax      \n\t"\
		"addl $8, %%esp       "\
		:: "a"(irqfunc), "b"((uint32_t) 0) : "memory");\
	__asm__ __volatile__("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	__asm__ goto(\
		".intel_syntax noprefix\n\t"\
		"mov eax, offset %l[startofISR]\n\t"\
		"mov [ebx], eax\n\t"\
		".att_syntax"\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}

/* Macro to create sotfware interrupt handling functions. */
/* It will call "intfunc" with the register context and the interrupt number as parameters. */
#define DEFINTWRAPPER(intnum)\
void *int##intnum##handler(void)\
{\
	volatile void *addr;\
	__asm__ goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	__asm__ __volatile__(".align 16" ::: "memory");\
	startofISR:\
	__asm__ __volatile__("pushal\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n\tcld" ::: "memory");\
	__asm__ __volatile__(\
		"pushl %%ds       \n\t"\
		"pushl %%es       \n\t"\
		"movw $16, %%cx   \n\t"\
		"movw %%cx, %%ds  \n\t"\
		"movw %%cx, %%es  \n\t"\
		"pushl %%ebp      \n\t"\
		"addl $4, (%%esp) \n\t"\
		"pushl %%ebx      \n\t"\
		"call *%%eax      \n\t"\
		"addl $8, %%esp       "\
		:: "a"(intfunc), "b"((uint32_t) intnum) : "memory");\
	__asm__ __volatile__("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	__asm__ goto(\
		".intel_syntax noprefix         \n\t"\
		"mov eax, offset %l[startofISR] \n\t"\
		"mov [ebx], eax                 \n\t"\
		".att_syntax                        "\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}

/*  Macro to create exception handling functions, for exceptions with error code. */
/* It will call intfunc_err, with the error code, the register context, and the interrupt number as parameters. */
#define DEFINTWRAPPER_ERR(intnum)\
void *int##intnum##handler(void)\
{\
	volatile void *addr;\
	__asm__ goto("jmp %l[endofISR]" ::: "memory" : endofISR);\
	__asm__ __volatile__(".align 16" ::: "memory");\
	startofISR:\
	__asm__ __volatile__(\
		"pushal                \n\t"\
		"pushl %%ebp           \n\t"\
		"movl %%esp, %%ebp     \n\t"\
		"pushl %%ds            \n\t"\
		"pushl %%es            \n\t"\
		"movw $16, %%cx        \n\t"\
		"movw %%cx, %%ds       \n\t"\
		"movw %%cx, %%es       \n\t"\
		"movl 36(%%ebp), %%edx \n\t"\
		"movl %%ebp, %%esi     \n\t"\
		"addl $32, %%esi       \n\t"\
		"movl %%esi, %%edi     \n\t"\
		"addl $4, %%edi        \n\t"\
		"movl $11, %%ecx       \n\t"\
		"std                   \n\t"\
		"rep movsl             \n\t"\
		"add $4, %%esp         \n\t"\
		"cld                       "\
		::: "memory");\
	__asm__ __volatile__(\
		"pushl %%edx       \n\t"\
		"pushl %%ebp       \n\t"\
		"addl $8, (%%esp)  \n\t"\
		"pushl %%ebx       \n\t"\
		"call *%%eax       \n\t"\
		"addl $12, %%esp       "\
		:: "a"(intfunc_err), "b"((uint32_t) intnum) : "memory");\
	__asm__ __volatile__("popl %%es\n\tpopl %%ds\n\tleave\n\tpopal\n\tiret" ::: "memory");\
	endofISR:\
	__asm__ goto(\
		".intel_syntax noprefix         \n\t"\
		"mov eax, offset %l[startofISR] \n\t"\
		"mov [ebx], eax                 \n\t"\
		".att_syntax                        "\
		:: "b"(&addr) : "eax", "memory" : startofISR);\
	return((void *) addr);\
}
//IRQs (the 16 IRQs the PIC has)
void *irq0handler(void);
void *irq1handler(void);
void *irq2handler(void);
void *irq3handler(void);
void *irq4handler(void);
void *irq5handler(void);
void *irq6handler(void);
void *irq7handler(void);
void *irq8handler(void);
void *irq9handler(void);
void *irq10handler(void);
void *irq11handler(void);
void *irq12handler(void);
void *irq13handler(void);
void *irq14handler(void);
void *irq15handler(void);
 
//exceptions without error code
void *int0handler(void);//division by 0
void *int1handler(void);//debug
void *int2handler(void);//NMI
void *int3handler(void);//breakpoint
void *int4handler(void);//INTO
void *int5handler(void);//BOUND
void *int6handler(void);//invalid opcode
void *int7handler(void);//coprocessor not available
void *int9handler(void);//coprocessor segment overrun
void *int16handler(void);//coprocessor error
 
//exceptions with error code
void *int8handler(void);//double fault
void *int10handler(void);//TSS error
void *int11handler(void);//segment not present
void *int12handler(void);//stack fault
void *int13handler(void);//general protection fault
void *int14handler(void);//page fault
 
//software interrupts
void *int0x80handler(void);//system call
#endif