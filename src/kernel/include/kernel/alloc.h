#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H 1

#define KERNEL_HEAP_START 	0x800000
#define KERNEL_HEAP_INIT	0x100000
#define HEAP_INDEX			0x20000
#define HEAP_MAGIC			0xDEADFADE
#define HEAP_MIN_SIZE		0x70000

typedef struct heap_struct
{
	void * index;
	uintptr_t start;
	uintptr_t end;
	uintptr_t max;
	uint8_t supervisor;
	uint8_t readonly;
} heap_t;

heap_t *create_heap(uintptr_t start, uintptr_t end, uintptr_t max, uint8_t s, uint8_t ro);
void *khalloc(uint32_t size, uint8_t align, heap_t * heap);
void khfree(void *p, heap_t *heap);

#endif