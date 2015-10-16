#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H value


#include <kernel/ord.h>

#define KERNEL_HEAP_START	0x00800000
#define KERNEL_HEAP_INIT	0x00100000
#define KERNEL_HEAP_MAGIC 	0xDEADFADE
#define KERNEL_HEAP_INDEX	0x00020000
#define KERNEL_HEAP_MIN		0x00070000
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

typedef struct
{
	ordered_array_t index;
	uintptr_t start;
	uintptr_t end;
	uintptr_t max;
	uint8_t supervisor;
	uint8_t readonly;
} heap_t;

uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t* phys);

uintptr_t kmalloc(size_t size);
uintptr_t kmalloc_a(size_t size);
uintptr_t kmalloc_p(size_t size, uintptr_t *phys);
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys);

void kfree(void *p);

heap_t *create_heap(uintptr_t start, uint32_t end, uint32_t max, uint8_t s, uint8_t ro);
void *alloc(size_t size, uint8_t align, heap_t *heap);
void free(void *, heap_t *heap);

#endif