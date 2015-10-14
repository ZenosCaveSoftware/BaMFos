#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H value


#include <kernel/ord.h>

#define KERNEL_HEAP_START	0x00800000
#define KERNEL_HEAP_END		0x20000000
#define KERNEL_HEAP_MAGIC 	0xDEADFADE


typedef struct
{
    uint32_t magic;
    uint8_t is_hole;
    uint32_t size;
} header_t;

typedef struct
{
    uint32_t magic;
    header_t *header;
} footer_t;


uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t* phys);

uintptr_t kmalloc(size_t size);
uintptr_t kmalloc_a(size_t size);
uintptr_t kmalloc_p(size_t size, uintptr_t *phys);
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys);

void kfree(void *p);

#endif