#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>

struct idt_entry_struct
{
	uint16_t	base_low;
	uint16_t	selector;
	uint8_t		zero;
	uint8_t		flags;
	uint16_t	base_high;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

#endif