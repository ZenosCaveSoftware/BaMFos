#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/ord.h>
#include <kernel/alloc.h>

extern uint32_t end;
extern page_directory_t *kernel_directory;

uintptr_t placement_address = (uintptr_t)&end;
heap_t *kernel_heap = (heap_t*) NULL;

#define bin_child_left(p) (2 * (p) + 1)
#define bin_child_right(p) (2 * (p + 1))

heap_t *create_heap(uintptr_t start, uintptr_t end, uintptr_t max, uint8_t s, uint8_t ro)
{
	return (heap_t *) NULL;
}

void *khalloc(uint32_t size, uint8_t align, heap_t *heap)
{
	return (void *) NULL;
}

void khfree(void *p, heap_t *heap)
{
}