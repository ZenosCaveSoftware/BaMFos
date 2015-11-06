#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H 1

#define KERNEL_HEAP_START 	0xC000000
#define KERNEL_HEAP_INIT	0x10000
#define KERNEL_HEAP_END		0xCFFF000
#define HEAP_INDEX			0x20000
#define HEAP_MAGIC			0xDEADFADE
#define HEAP_MIN_SIZE		0x70000

typedef void *type_t;

typedef struct heap_header_struct
{
	uint32_t size;
	uint8_t is_free;
	uintptr_t next;
} __attribute__((packed)) heap_header_t;

typedef struct heap_footer_struct
{
	uint32_t size;
	uintptr_t prev;
} __attribute__((packed)) heap_footer_t;


//unused {{{
typedef struct heap_bin_struct
{
	uintptr_t next;
	uint32_t size_index;
	type_t *val;
} __attribute__((packed)) heap_bin_t;
// }}}

typedef struct heap_struct
{
	//heap_bin_t *bins;
	heap_header_t *head;
	uintptr_t start;
	uintptr_t end;
	uintptr_t max;
	uint8_t supervisor;
	uint8_t readonly;
} __attribute__((packed)) heap_t;

heap_t *create_heap(uintptr_t start, uintptr_t end, uintptr_t max, uint8_t s, uint8_t ro);
void *khalloc(uint32_t size, uint8_t align, heap_t * heap);
void khfree(void *p, heap_t *heap);

#endif