#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/alloc.h>
#include <kernel/tty.h>

extern uint32_t end;
extern page_directory_t *kernel_directory;

uintptr_t placement_address = (uintptr_t)&end;
heap_t *kernel_heap = (heap_t*) NULL;

static void free_block(heap_t *heap,uintptr_t u_data);
static void alloc_block(heap_t *heap, uint32_t size);

static uintptr_t next_free_block(uintptr_t u_data);
static uintptr_t next_contig_block(uintptr_t u_data);
static uintptr_t prev_free_block(uintptr_t u_data);
static uintptr_t prev_contig_block(uintptr_t u_data);
static uintptr_t coalesce(heap_header_t *head_ptr);

heap_t *create_heap(uintptr_t start, uintptr_t end, uintptr_t max, uint8_t s, uint8_t ro)
{

	heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
	terminal_writestring("heap alloc'd  ...\n");
	assert(!start&0xFFF);
	assert(!end&0xFFF);

	heap->head = (heap_header_t *)(start = (start + sizeof(heap_t) + 0xFFF) & ~(0xFFF));
	terminal_writestring("            head addr set ...\n");
	
	heap->head->size = end - start;
	heap->head->is_free = 1;
	heap->head->next = NULL;
	heap->start = start;
	heap->end = end;
	heap->max = max;
	heap->supervisor = s;
	heap->readonly = ro;

	return heap;
}

void *khalloc(uint32_t size, uint8_t align, heap_t *heap)
{
	return (void *) NULL;
}

void khfree(void *p, heap_t *heap)
{
}

uintptr_t next_free_block(uintptr_t u_data)
{
	heap_header_t *head_ptr = (heap_header_t*)((uint32_t) u_data - sizeof(heap_header_t));
	return (head_ptr->is_free) ? head_ptr->next : (uintptr_t) NULL;
}

uintptr_t next_contig_block(uintptr_t u_data)
{
	heap_header_t *head_ptr =  (heap_header_t *)((uint32_t) u_data - sizeof(heap_header_t));
	return (uintptr_t) (head_ptr->size + (uint32_t) head_ptr);
}

uintptr_t prev_free_block(uintptr_t u_data)
{
	heap_header_t *head_ptr = (heap_header_t *)((uint32_t) u_data - sizeof(heap_header_t));
	heap_footer_t *foot_ptr = (heap_footer_t *)(head_ptr->size + 
								(uint32_t) head_ptr 
								- sizeof(heap_footer_t));
	return (head_ptr->is_free) ? foot_ptr->prev : (uintptr_t) NULL;
}

uintptr_t prev_contig_block(uintptr_t u_data)
{
	heap_header_t *head_ptr = (heap_header_t*)((uint32_t) u_data - sizeof(heap_header_t));
	heap_footer_t *prev_foot_ptr = (heap_footer_t *)((uint32_t) head_ptr - sizeof(heap_header_t));
	return (uintptr_t) ((uint32_t) head_ptr - prev_foot_ptr->size);
}

uintptr_t coalesce(heap_header_t *head_ptr)
{	
	heap_header_t * next_ptr;
	heap_header_t * prev_ptr;
	heap_footer_t * foot_ptr;

	if ((next_ptr = (heap_header_t *) next_contig_block(
			(uintptr_t)((uint32_t) head_ptr
			 + sizeof(heap_header_t)))) != NULL && next_ptr->is_free)
	{
		(foot_ptr = (heap_footer_t *)(next_ptr->size
			+ (uint32_t) next_ptr
			- sizeof(heap_footer_t)))->prev 
		= ((heap_footer_t *)(head_ptr->size 
			+ (uint32_t) head_ptr 
			- sizeof(heap_footer_t)))->prev;
		head_ptr->size += next_ptr->size;
		foot_ptr->size = head_ptr->size; 
	}
	if ((prev_ptr = (heap_header_t *) prev_contig_block(
			(uintptr_t)((uint32_t) head_ptr
			 + sizeof(heap_header_t)))) != NULL && next_ptr->is_free)
	{
		(foot_ptr = (heap_footer_t *)(head_ptr->size 
			+ (uint32_t) head_ptr 
			- sizeof(heap_footer_t)))->prev 
		= ((heap_footer_t *)(prev_ptr->size
			+ (uint32_t) prev_ptr
			- sizeof(heap_footer_t)))->prev;
		prev_ptr->size += head_ptr->size;
		foot_ptr->size = prev_ptr->size;
		
	}
	return (uintptr_t) ((prev_ptr && prev_ptr->is_free) ? prev_ptr : head_ptr);
}