#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

struct idt_entry_struct
{
	uint16_t	base_low;
	uint16_t	selector;
	uint8_t		reserved0;
	uint8_t		flags;
	uint16_t	base_mid;
	uint32_t	base_high;
	uint32_t 	reserved1;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
	uint16_t	limit;
	uintptr_t	base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

//idt
void initialize_idt();

extern void idt_flush(uint32_t);

static void fillidte(int, uint16_t, void *, uint8_t, uint8_t);


//isr & irq
void initialize_irq();

void pic_remap(uint8_t, uint8_t);

#endif