#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <kernel/mem.h>
#include <kernel/paging.h>
#include <kernel/tty.h>

extern void *end;
extern page_directory_t *kernel_directory;
uintptr_t placement_address = (uintptr_t)&end;
heap_t *kheap = NULL;
uintptr_t _kmalloc(size_t size, uint8_t align, uintptr_t *phys)
{

	if (kheap)
    {
        void *addr = alloc(size, (uint8_t)align, kheap);
        if (phys)
        {
            page_t *page = get_page((uintptr_t) addr, 0, kernel_directory);
            *phys = page->frame*0x1000 + (uintptr_t)addr & 0xFFF;
        }
        return (uintptr_t)addr;
    }

	if(align && (placement_address & 0xFFFFF000))
	{
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}	
	if(phys)
	{
		*phys = placement_address;
	}
	uintptr_t addr = placement_address;
	placement_address += size;
	return addr;
}

uintptr_t kmalloc(size_t size) { return _kmalloc(size, 0, NULL); }
uintptr_t kmalloc_a(size_t size) { return _kmalloc(size, 1, NULL); }
uintptr_t kmalloc_p(size_t size, uintptr_t *phys) { return _kmalloc(size, 0, phys); }
uintptr_t kmalloc_ap(size_t size, uintptr_t *phys) { return _kmalloc(size, 1, phys); }

void kfree(void *p) 
{ 
	free(p, kheap);
}

static int32_t find_smallest_hole(uint32_t size, uint8_t page_align, heap_t *heap)
{
    // Find the smallest hole that will fit.
    uint32_t iterator = 0;
    while (iterator < heap->index.size)
    {
        header_t *header = (header_t *)lookup_ordered_array(iterator, &heap->index);
        // If the user has requested the memory be page-aligned
        if (page_align > 0)
        {
            // Page-align the starting point of this header.
            uint32_t location = (uint32_t)header;
            int32_t offset = 0;
            if ((location+sizeof(header_t)) & 0xFFFFF000 != 0)
                offset = 0x1000 /* page size */  - (location+sizeof(header_t))%0x1000;
            int32_t hole_size = (int32_t)header->size - offset;
            // Can we fit now?
            if (hole_size >= (int32_t)size)
                break;
        }
        else if (header->size >= size)
            break;
        iterator++;
    }
    // Why did the loop exit?
    if (iterator == heap->index.size)
        return -1; // We got to the end and didn't find anything.
    else
        return iterator;
}

static int8_t header_t_comperator(void *a, void *b)
{
	return (((header_t*)a)->size < ((header_t*)b)->size)?1:0;
}

static void expand(size_t new_size, heap_t *heap)
{
	assert(new_size > heap->end - heap->start);

	if(new_size & 0xFFFFF000 != 0)
	{
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}

	assert(heap->start+new_size <= heap->max);

	size_t old_size = (size_t)(heap->end - heap->start);
	size_t i = old_size;
	while(i < new_size)
	{
		alloc_frame(get_page(heap->start + i, 1, kernel_directory),
			(heap->supervisor)?1:0, (heap->readonly)?0:1);
		i += 0x1000;
	}

	heap->end = heap->start + new_size;
}

static size_t contract(size_t new_size, heap_t *heap)
{
	assert(new_size < heap->end - heap->start);

	if (new_size&0x1000)
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    if (new_size < KERNEL_HEAP_MIN)
        new_size = KERNEL_HEAP_MIN;

    size_t old_size = (size_t)(heap->end - heap->start);
    size_t i = old_size - 0x1000;
    while (new_size < i)
    {
        free_frame(get_page(heap->start + i, 0, kernel_directory));
        i -= 0x1000;
    }
    heap->end = heap->start + new_size;
    return new_size;
}

heap_t *create_heap(uintptr_t start, uint32_t end, uint32_t max, uint8_t s, uint8_t ro) 
{
	terminal_writestring("\n-- Allocating heap struct...\t");
    heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));

	assert(start % 0x1000 == 0);
	assert(end % 0x1000 == 0);
	

    terminal_writestring("\n-- placing heap array...\t");    
	heap->index = place_ordered_array( (void*)start, KERNEL_HEAP_INDEX, &header_t_comperator);

	start += sizeof(proto_t)*KERNEL_HEAP_INDEX;

	if(start & 0xFFFFF000)
	{
		start &= 0xFFFFF000;
		start += 0x1000;
	}

	heap->start = start;
	heap->end = end;
	heap->max = max;
	heap->supervisor = s;
	heap->readonly = ro;

	header_t *hole = (header_t *)start;
	hole->size = end - start;
	hole->magic =  KERNEL_HEAP_MAGIC;
	hole->is_hole = 1;

    terminal_writestring("\n-- Insert initial hole into heap\t"); 
	insert_ordered_array((void *)hole, &heap->index);

	return heap;
}


void *alloc(size_t size, uint8_t page_align, heap_t *heap)
{

    size_t new_size = size + sizeof(header_t) + sizeof(footer_t);    // Find the smallest hole that will fit.
    int32_t iterator = find_smallest_hole(new_size, page_align, heap);

    if (iterator == -1)
    {
        uint32_t old_length = heap->end - heap->start;
        uint32_t old_end = heap->end;

        expand(old_length+new_size, heap);
        uint32_t new_length = heap->end - heap->start;

        iterator = 0;
        uint32_t idx = -1; uint32_t value = 0x0;
        while (iterator < heap->index.size)
        {
            uint32_t tmp = (uint32_t)lookup_ordered_array(iterator, &heap->index);
            if (tmp > value)
            {
                value = tmp;
                idx = iterator;
            }
            iterator++;
        }

        if (idx == -1)
        {
            header_t *header = (header_t *)old_end;
            header->magic =  KERNEL_HEAP_MAGIC;
            header->size = new_length - old_length;
            header->is_hole = 1;
            footer_t *footer = (footer_t *) (old_end + header->size - sizeof(footer_t));
            footer->magic =  KERNEL_HEAP_MAGIC;
            footer->header = header;
            insert_ordered_array((void*)header, &heap->index);
        }
        else
        {
            header_t *header = lookup_ordered_array(idx, &heap->index);
            header->size += new_length - old_length;
            footer_t *footer = (footer_t *) ( (uint32_t)header + header->size - sizeof(footer_t) );
            footer->header = header;
            footer->magic =  KERNEL_HEAP_MAGIC;
        }
        return alloc(size, page_align, heap);
    }

    header_t *orig_hole_header = (header_t *)lookup_ordered_array(iterator, &heap->index);
    uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
    uint32_t orig_hole_size = orig_hole_header->size;
    if (orig_hole_size-new_size < sizeof(header_t)+sizeof(footer_t))
    {
        size += orig_hole_size-new_size;
        new_size = orig_hole_size;
    }

    // If we need to page-align the data, do it now and make a new hole in front of our block.
    if (page_align && orig_hole_pos&0xFFFFF000)
    {
        uint32_t new_location   = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
        header_t *hole_header = (header_t *)orig_hole_pos;
        hole_header->size     = 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
        hole_header->magic    =  KERNEL_HEAP_MAGIC;
        hole_header->is_hole  = 1;
        footer_t *hole_footer = (footer_t *) ( (uint32_t)new_location - sizeof(footer_t) );
        hole_footer->magic    =  KERNEL_HEAP_MAGIC;
        hole_footer->header   = hole_header;
        orig_hole_pos         = new_location;
        orig_hole_size        = orig_hole_size - hole_header->size;
    }
    else
    {
        remove_ordered_array(iterator, &heap->index);
    }

    header_t *block_header  = (header_t *)orig_hole_pos;
    block_header->magic     =  KERNEL_HEAP_MAGIC;
    block_header->is_hole   = 0;
    block_header->size      = new_size;
    footer_t *block_footer  = (footer_t *) (orig_hole_pos + sizeof(header_t) + size);
    block_footer->magic     =  KERNEL_HEAP_MAGIC;
    block_footer->header    = block_header;

    if (orig_hole_size - new_size > 0)
    {
        header_t *hole_header = (header_t *) (orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
        hole_header->magic    =  KERNEL_HEAP_MAGIC;
        hole_header->is_hole  = 1;
        hole_header->size     = orig_hole_size - new_size;
        footer_t *hole_footer = (footer_t *) ( (uint32_t)hole_header + orig_hole_size - new_size - sizeof(footer_t) );
        if ((uint32_t)hole_footer < heap->end)
        {
            hole_footer->magic =  KERNEL_HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        insert_ordered_array((void*)hole_header, &heap->index);
    }
    
    return (void *) ( (uint32_t)block_header+sizeof(header_t) );
}

void free(void *p, heap_t *heap)
{
	if(p == 0) return;

	header_t *header = (header_t*) ( (uint32_t)p - sizeof(header_t) );
	footer_t *footer = (footer_t*) ( (uint32_t)header + header->size - sizeof(footer_t) );

	assert(header->magic ==  KERNEL_HEAP_MAGIC);
	assert(footer->magic ==  KERNEL_HEAP_MAGIC);

	header->is_hole=1;

	char do_add = 1;
	footer_t * left_test = (footer_t*) ( ( uint32_t) header - sizeof(footer_t) );

	if(left_test->magic ==  KERNEL_HEAP_MAGIC &&
	   left_test->header->is_hole)
	{
		uint32_t cache_size = header->size;
		header = left_test->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	} 

	header_t *right_test = (header_t*) ( (uint32_t) footer + sizeof(footer_t) );
	if(right_test->magic ==  KERNEL_HEAP_MAGIC &&
	   right_test->is_hole)
	{
		header->size += right_test->size;
		left_test = (footer_t*) ( (uint32_t) right_test +
								  right_test->size - sizeof(footer_t) );
		footer = left_test;

		uint32_t iterator = 0;
		while( (iterator < heap->index.size) &&
				(lookup_ordered_array(iterator, &heap->index) != (void*)right_test) )
			iterator++;

		assert(iterator < heap->index.size);

		remove_ordered_array(iterator, &heap->index);
	}

	if( (uint32_t)footer+sizeof(footer_t) == heap->end )
	{
		size_t old_length = heap->end - heap->start;
		size_t new_length = contract( (uint32_t) header - heap->start, heap);


		if(header->size - old_length > new_length)
		{
			header->size -= old_length - new_length;
			footer = (footer_t*) ( (uint32_t) header + header->size - sizeof(footer_t) );

		}
		else
		{
			uint32_t iterator = 0;
			while( (iterator < heap->index.size) &&
					(lookup_ordered_array(iterator, &heap->index) != (void*)right_test) )
				iterator++;

			if(iterator < heap->index.size)
				remove_ordered_array(iterator, &heap->index);
		}
	}	

	if(do_add)
		insert_ordered_array((void*)header, &heap->index);
}