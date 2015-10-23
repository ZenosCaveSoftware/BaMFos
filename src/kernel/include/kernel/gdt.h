#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>

// This structure contains the value of one GDT entry.
// We use the attribute 'packed' to tell GCC not to change
// any of the alignment in the structure.
typedef struct gdt_entry_struct
{
   uint16_t limit_low;           // The lower 16 bits of the limit.
   uint16_t base_low;            // The lower 16 bits of the base.
   uint8_t  base_middle;         // The next 8 bits of the base.
   uint8_t  access;              // Access flags, determine what ring this segment can be used in.
   uint8_t  granularity;
   uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed)) gdt_entry_t; 

typedef struct gdt_source_struct
{
	uint32_t base;
	uint32_t limit;
	uint8_t type;
}__attribute__((packed)) gdt_t;

typedef struct gdt_ptr_struct
{
   uint16_t limit;
   uintptr_t base;
} __attribute__((packed)) gdt_ptr_t; 

//gdt
void initialize_gdt();

extern void gdt_flush(uint32_t);

static void fillgdte(int32_t num, gdt_t, bool);

#endif