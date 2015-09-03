#ifndef _KERNEL_DTS_H
#define _KERNEL_DTS_H

#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>

#include <kernel/gdt.h>
#include <kernel/tss.h>
#include <kernel/idt.h>
#include <kernel/isr.h>

void initialize_descriptor_tables();
//gdt
static void initialize_gdt();

extern void gdt_flush(uint32_t);

static void fillgdte(gdt_entry_t*, gdt_t, bool);

//tss
static void write_tss(gdt_entry_t*, uint16_t, uint32_t);

extern void tss_flush();

//idt
static void initialize_idt();

extern void idt_flush(uint32_t);

static void fillidte(idt_entry_t*, uint16_t, void *, uint8_t, uint8_t);

//isr & irq
void initialize_irq();

void pic_remap(uint8_t, uint8_t);

#endif