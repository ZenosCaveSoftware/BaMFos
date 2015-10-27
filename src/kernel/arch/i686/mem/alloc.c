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

static int32_t find_smallest_fit(uint32_t size, uint8_t align, heap_t *heap);
static int8_t header_compare(void *a, void *b);
static void expand(uint32_t size, heap_t * heap);
static uint32_t contract(uint32_t size, heap_t *heap);

heap_t *create_heap(uintptr_t start, uintptr_t end, uintptr_t max, uint8_t s, uint8_t ro)
{
	heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

	assert(!start&0xFFF);
	assert(!end&0xFFF);

	heap->index = place_ordered_array(start, HEAP_INDEX, &header_compare);
	start += HEAP_INDEX * sizeof(cast_t);

	if(start & 0xFFF)
	{
		start &= 0xFFFFF000;
		start += 0x1000;
	}

	heap->start = start;
	heap->end = end;
	heap->max = max;
	heap->supervisor = s;
	heap->readonly = ro;

	heap_header_t *hole = (heap_header_t *)start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;

	insert_ordered_array((void *)hole, &heap->index);

	return heap;
}

void *khalloc(uint32_t size, uint8_t align, heap_t *heap)
{
	uint32_t comp_size = size + sizeof(heap_header_t) + sizeof(heap_footer_t);
	int32_t i = find_smallest_fit(comp_size, align, heap);

	if (i == -1)
	{
		uint32_t prev_len = heap->end - heap->start;
		uintptr_t prev_end = heap->end;
		expand(prev_len + comp_size, heap);
		uint32_t new_len = heap->end - heap->start;

		i = 0;
		uint32_t idx = -1; uint32_t val = 0x0;
		while(i < heap->index.size)
		{
			uint32_t tmp = (uint32_t)lookup_ordered_array(i, &heap->index);
			if(tmp > val)
			{
				val = tmp;
				idx = i;
			}
			i++;
		}

		if(idx == -1)
		{
			heap_header_t *header = (heap_header_t *)prev_end;
			header->magic = HEAP_MAGIC;
			header->size = new_len - prev_len;
			header->is_hole = 1;

			heap_footer_t *footer = (heap_footer_t *)(prev_end + header->size - sizeof(heap_footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			insert_ordered_array((void *)header, &heap->index);
		}
		else
		{
			heap_header_t *header = lookup_ordered_array(idx, &heap->index);
			header->size += new_len - prev_len;

			heap_footer_t *footer = (heap_footer_t *) ((uintptr_t) header + header->size - sizeof(heap_footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}

		return khalloc(size, align, heap);
	}

	heap_header_t *orig_header = (heap_header_t *)lookup_ordered_array(i, &heap->index);
	uintptr_t orig_pos = (uintptr_t) orig_pos;
	uint32_t orig_size = orig_header->size;

	if(orig_size - comp_size < sizeof(heap_header_t) + sizeof(heap_footer_t))
	{
		size += orig_size - comp_size;
		comp_size = orig_size;
	}
	else if(align && orig_pos & 0xFFFFF000)
	{
		uint32_t new_loc = orig_pos&0xFFFFF000 + 0x1000 + sizeof(heap_header_t);
		heap_header_t *hole = (heap_header_t *)orig_pos;
		hole->magic = HEAP_MAGIC;
		hole->size = 0x1000;
		hole->is_hole = 1;

		heap_footer_t *hole_footer = (heap_footer_t *)(new_loc - sizeof(heap_footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole;

		orig_pos = new_loc;
		orig_size -= hole->size; 
	}
	else
	{
		remove_ordered_array(i, &heap->index);
	}

	heap_header_t *block = (heap_header_t *) orig_pos;
	block->magic = HEAP_MAGIC;
	block->size = comp_size;
	block->is_hole = 0;

	heap_footer_t *block_footer = (heap_footer_t *)(orig_pos + sizeof(heap_header_t) + size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block;


	if(orig_size > comp_size)
	{
		heap_header_t *hole = (heap_header_t *) (orig_pos + sizeof(heap_header_t) + size);
		hole->magic = HEAP_MAGIC;
		hole->size = orig_size - comp_size;
		hole->is_hole = 1;

		heap_footer_t *hole_footer = (heap_footer_t *)(hole + orig_size - comp_size - sizeof(heap_footer_t));
		if((uintptr_t) hole_footer < heap->end)
		{
			hole_footer->magic;
			hole_footer->header = hole;
		}

		insert_ordered_array((void *)hole, &heap->index);
	}

	return (void *)((uintptr_t)block+sizeof(heap_header_t));
}

void khfree(void *p, heap_t *heap)
{
	if (__builtin_expect(p == NULL, 0)) {
		return;
	}

	heap_header_t *header = (heap_header_t *) ( (uintptr_t)p - sizeof(heap_header_t) );
	heap_footer_t *footer = (heap_footer_t *) ( (uintptr_t)header + header->size - sizeof(heap_footer_t) );

	assert(header->magic == HEAP_MAGIC);
	assert(footer->magic == HEAP_MAGIC);

	header->is_hole = 1;

	char free_hole = 1;

	heap_footer_t *test_footer = (heap_footer_t*) ( (uintptr_t)header - sizeof(heap_footer_t) );
    if(test_footer->magic == HEAP_MAGIC &&
       test_footer->header->is_hole)
    {
    	uint32_t cache_size = header->size;
    	header = test_footer->header;
    	footer->header = header;
    	header->size += cache_size;
    	free_hole = 0;
    }

    heap_header_t *test_header = (heap_header_t *)( (uintptr_t)footer + sizeof(heap_footer_t) );
    if(test_header->magic == HEAP_MAGIC &&
    	test_header->ishole)
    {
    	header->size += test_header->size;
    	test_footer = (heap_footer_t *) ( (uintptr_t) test_header + 
    									   test_header->size - sizeof(heap_footer_t) );

  		footer = test_footer;
  		uint32_t i = 0;
  		while( (i < heap->index.size) &&
  			   (lookup_ordered_array(i, &heap->index) != (void *)test_header) )
  			i++;

  		assert(i < heap->index.size);

  		remove_ordered_array(i, &heap->index);
    }

    if( (uintptr_t)footer + sizeof(heap_footer_t) == heap->end )
    {
    	uint32_t old_len = heap->end - heap->start;
    	uint32_t new_len = contract( (uint32_t)header - heap->start, heap);

    	if(header->size + new_len > old_len)
    	{
    		header->size -= old_len - new_len;
    		footer = (heap_footer_t *) ( (uintptr_t)header + header->size -sizeof(heap_footer_t) );
    		
    	}
    }
}
int32_t find_smallest_fit(uint32_t size, uint8_t align, heap_t *heap)
{
	uint32_t i = 0;
	while(i < heap->index.size)
	{
		heap_header_t *header = (heap_header_t *)lookup_ordered_array(i, &heap->index);
		if(align)
		{
			uintptr_t location = (uintptr_t)header;
			int8_t offset = 0;
			if ((location + sizeof(heap_header_t)) & 0xFFF)
			{
				offset = 0x1000 - ((location + sizeof(heap_header_t)) &0xFFF);
			}
			if((int32_t)header->size - offset >= (int32_t)size)
			{
				break;
			}	
		}
		else if (header->size >= size)
		{
			break;
		}
		i++;
	}

	return (i == heap->index.size) ? -1 : i;
}


int8_t header_compare(void * a, void *b)
{
	return (((heap_header_t*)a)->size < ((heap_header_t*)b)->size)?1:0;
}

void expand(uint32_t size, heap_t *heap)
{
	assert(size > heap->end - heap->start);

	if (size & 0xFFF)
	{
		size &= 0xFFFFF000;
		size += 0x1000;
	}

	assert(heap->start + size <= heap->max);

	uint32_t i =  heap->end - heap->start;
		while (i < size)
		{
			alloc_frame( get_page(heap->start+i, 1, kernel_directory), (heap->supervisor)?1:0, (heap->readonly)?0:1);
			i += 0x1000;
		}
	heap->end = heap->start + size;
}

uint32_t contract(uint32_t size, heap_t *heap)
{
	assert(size < heap->end - heap->start);

	if(size&0xFFF)
	{
		size &= 0xFFFFF000;
		size += 0x1000;
	}

	if(size < HEAP_MIN_SIZE)
	{
		size = HEAP_MIN_SIZE;
	}

	uint32_t i = heap->end - heap->start - 0x1000;
	while(size < i)
	{
		free_frame(get_page(heap->start+i, 0, kernel_directory));
		i -= 0x1000;
	}
	heap->end = heap->start + size;
	return size; 
}