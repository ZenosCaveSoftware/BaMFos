#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <kernel/ord.h>
#include <kernel/mem.h>

int8_t statndard_order_comperator(proto_t a, proto_t b)
{
	return (a<b)?1:0;
}

ordered_array_t create_ordered_array(uint32_t max_size, binary_comperator_t comp)
{
	ordered_array_t ret;
	ret.values = (void *)kmalloc(max_size*sizeof(proto_t));
	memset(ret.values, 0, max_size*sizeof(proto_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.comp = comp;
}

ordered_array_t place_ordered_array(void *addr, uint32_t max_size, binary_comperator_t comp)
{
	ordered_array_t ret;
	ret.values = (proto_t*)addr;
	memset(ret.values, 0, max_size*sizeof(proto_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.comp = comp;
}
void destroy_ordered_array(ordered_array_t *array)
{
	kfree(array->values);
}

int insert_ordered_array(proto_t item, ordered_array_t *array)
{
	assert(array->comp);
	uint32_t i = 0;

	if(array->size != array->max_size) 
	{
		while(i < array->size && array->comp(array->values[i], item)) i++;
		if(i == array->size)
		{
			array->values[array->size++] = item;
		}
		
		else{
			proto_t tmp = array->values[i];
			proto_t tmp2;
			array->values[i] = item;
			while(i < array->size && array->values[i])
			{
				i++;
				tmp2 = array->values[i];
				array->values[i] = tmp;
				tmp = tmp2;
			}
			array->size++;
		}	
		return 0;
	}
	return 1;	
}

proto_t lookup_ordered_array(uint32_t i, ordered_array_t *array) 
{
	assert(i < array->size);
	return array->values[i];
}

void remove_ordered_array(uint32_t i, ordered_array_t *array)
{
	while(i < array->size)
	{
		array->values[i] = array->values[++i];
	}
	array->size--;
}