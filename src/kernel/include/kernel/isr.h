#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>

typedef struct registers
{
   uint32_t gs, fs, es, ds;                  		// Segment selectors
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    					// Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; 			// Pushed by the processor automatically.
} registers_t;

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


typedef void *(*isr_handler_t)(registers_t *);	/* interrupt service routine handler type */
typedef int *(*irq_handler_t)(registers_t *);	/* interrupt request handler type */


void register_isr_handler(uint8_t n, isr_handler_t handler);
void unregister_isr_handler(uint8_t n);

void register_irq_handler(uint8_t n, irq_handler_t handler);
void unregister_irq_handler(uint8_t n);

void irq_handler(registers_t *regs);
void fault_handler(registers_t *regs);

uint8_t pic_isnormalIRQ(uint32_t irqnum);
void pic_EOI(uint32_t irqnum);
void pic_EOI_spurious(uint32_t irqnum);

static uint16_t __pic_get_irq_reg(int ocw3);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif
