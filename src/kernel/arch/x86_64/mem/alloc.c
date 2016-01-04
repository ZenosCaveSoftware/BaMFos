#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <misc/panic.h>
#include <mem/mem.h>
#include <mem/paging.h>
#include <mem/alloc.h>
#include <io/tty.h>

extern uint64_t end;
extern page_directory_t *kernel_directory;

uintptr_t placement_address = (uintptr_t)&end;
heap_t *kernel_heap = (heap_t*) NULL;

uintptr_t next_free_block(uintptr_t u_data);
uintptr_t next_contig_block(uintptr_t u_data);
uintptr_t prev_free_block(uintptr_t u_data);
uintptr_t prev_contig_block(uintptr_t u_data);
//void coalesce(heap_header_t *head_ptr);

heap_t *create_heap(uintptr_t start, uintptr_t end, uintptr_t max, uint8_t s, uint8_t ro)
{
	heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
	assert(!start&0xFFF);
	assert(!end&0xFFF);
	
	heap->head = (heap_header_t *)start;
	heap->head->size = (uint64_t) end - (uint64_t) start;
	heap->head->is_free = 1;
	heap->head->next = NULL;
	heap->start = start;
	heap->end = end;
	heap->max = max;
	heap->supervisor = s;
	heap->readonly = ro;
	heap_footer_t *foot_ptr = NULL;
	(foot_ptr = ((heap_footer_t *)((uint64_t)end - sizeof(heap_footer_t))))->size = heap->head->size;
	foot_ptr->prev = NULL;
	return heap;
}

void *khalloc(uint64_t size, uint8_t align, heap_t *heap)
{
	printf("allocing  0x%x bytes\n", size);

	if(__builtin_expect(size == 0, 0))
	{
		return NULL;
	}

	heap_header_t * head_ptr = heap->head;
	heap_header_t * curr_ptr = head_ptr;

	while(head_ptr)
	{
		if(!head_ptr->next)
			break;
		head_ptr = (heap_header_t *)head_ptr->next;
	}
	while(!head_ptr->is_free)
	{
		head_ptr = (heap_header_t *) prev_contig_block((uintptr_t)((uint64_t)head_ptr + sizeof(heap_header_t)));
		if (!head_ptr)
			break;
	}
	while(head_ptr->size < size + sizeof(heap_header_t) + sizeof(heap_footer_t))
	{
		head_ptr = (heap_header_t *) prev_free_block((uintptr_t)((uint64_t)head_ptr + sizeof(heap_header_t)));
		if(!head_ptr)
			break;
	}
	if(!(head_ptr && head_ptr->is_free &&
		head_ptr->size >= size + sizeof(heap_header_t) + sizeof(heap_footer_t)))
	{
		PANIC("Out of heap!!!");	//Something is wrong...
	}

	heap_footer_t * new_foot = (heap_footer_t *)((uint64_t)head_ptr + head_ptr->size - size - (2 * sizeof(heap_footer_t)));
	heap_footer_t * foot_ptr = (heap_footer_t *)((uint64_t)head_ptr + head_ptr->size - sizeof(heap_footer_t));
	heap_header_t * new_head = (heap_header_t *)((uint64_t)new_foot + sizeof(heap_footer_t)); 

	new_head->size = size + sizeof(heap_header_t) + sizeof(heap_footer_t);
	new_head->is_free = 0;
	new_head->next = NULL;

	head_ptr->size -= new_head->size;
	
	new_foot->size = head_ptr->size;
	new_foot->prev = foot_ptr->prev;

	foot_ptr->size = new_head->size;
	foot_ptr->prev = NULL;

	return (void *)((uint64_t)new_head + sizeof(heap_header_t));
}

void khfree(void *p, heap_t *heap)
{
	if (__builtin_expect(p == NULL, 0)) 
	{
		return;
	}

	heap_header_t *head_ptr	= (heap_header_t *)((uint64_t) p - sizeof(heap_header_t));
	heap_header_t *next_ptr;
	heap_header_t *prev_ptr;
	heap_footer_t *foot_ptr;

	if(!head_ptr->is_free)
	{
		head_ptr->is_free = 1;
		next_ptr = (heap_header_t *) next_contig_block((uintptr_t) p);
		if (next_ptr != NULL && next_ptr->is_free)
		{
			foot_ptr = (heap_footer_t *)(next_ptr->size
				+ (uint64_t) next_ptr
				- sizeof(heap_footer_t));
			foot_ptr->prev = ((heap_footer_t *)
			(head_ptr->size + (uint64_t) head_ptr - sizeof(heap_footer_t)))->prev;

			head_ptr->next = next_ptr->next;
			head_ptr->size += next_ptr->size;
			foot_ptr->size = head_ptr->size; 
		}
		
		prev_ptr = (heap_header_t *) prev_contig_block((uintptr_t) p);
		if (prev_ptr != NULL && prev_ptr->is_free)
		{
			foot_ptr = (heap_footer_t *)(head_ptr->size 
				+ (uint64_t) head_ptr 
				- sizeof(heap_footer_t));
			foot_ptr->prev = ((heap_footer_t *)
			(prev_ptr->size +(uint64_t) prev_ptr - sizeof(heap_footer_t)))->prev;
			
			prev_ptr->next = head_ptr->next;
			prev_ptr->size += head_ptr->size;
			foot_ptr->size = prev_ptr->size;
		}
	}
}

uintptr_t next_free_block(uintptr_t u_data)
{
	heap_header_t *head_ptr = (heap_header_t*)((uint64_t) u_data - sizeof(heap_header_t));
	return (head_ptr->is_free) ? head_ptr->next : (uintptr_t) NULL;
}

uintptr_t next_contig_block(uintptr_t u_data)
{
	heap_header_t *head_ptr =  (heap_header_t *)((uint64_t) u_data - sizeof(heap_header_t));
	if((uint64_t) head_ptr >= kernel_heap->end || (uint64_t) head_ptr + head_ptr->size >= kernel_heap->end) return NULL;
	return (uintptr_t) (head_ptr->size + (uint64_t) head_ptr);
}

uintptr_t prev_free_block(uintptr_t u_data)
{
	heap_header_t *head_ptr = (heap_header_t *)((uint64_t) u_data - sizeof(heap_header_t));
	heap_footer_t *foot_ptr = (heap_footer_t *)(head_ptr->size + 
								(uint64_t) head_ptr 
								- sizeof(heap_footer_t));
	return (head_ptr->is_free) ? foot_ptr->prev : (uintptr_t) NULL;
}

uintptr_t prev_contig_block(uintptr_t u_data)
{
	heap_header_t *head_ptr = (heap_header_t*)((uint64_t) u_data - sizeof(heap_header_t));
	heap_footer_t *prev_foot_ptr = (heap_footer_t *)((uint64_t) head_ptr - sizeof(heap_header_t) - sizeof(heap_footer_t));
	if((uint64_t) prev_foot_ptr < kernel_heap->start || (uint64_t) head_ptr - prev_foot_ptr->size < kernel_heap->start) return NULL;
	return (uintptr_t)((uint64_t) head_ptr - prev_foot_ptr->size);
}


