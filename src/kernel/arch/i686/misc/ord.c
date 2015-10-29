#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <kernel/ord.h>
#include <kernel/mem.h>

int8_t lessthan_comperator(cast_t a, cast_t b)
{
	return (a<b)?1:0;
}


ordered_array_t create_ordered_array(uint32_t max, binary_comperator_t compare)
{
	ordered_array_t ret;
	ret.values = (void *)kmalloc(max*sizeof(cast_t));
	memset(ret.values, 0, max*sizeof(cast_t));
	ret.size = 0;
	ret.max = max;
	ret.compare = compare;
	return ret;
}

ordered_array_t place_ordered_array(uintptr_t addr, uint32_t max, binary_comperator_t compare)
{
	ordered_array_t ret;
	ret.values = (void *)addr;
	memset(ret.values, 0, max*sizeof(cast_t));
	ret.size = 0;
	ret.max = max;
	ret.compare = compare;
	return ret;
}

void destroy_ordered_array(ordered_array_t *arr)
{

}

uint8_t insert_ordered_array(cast_t val, ordered_array_t *arr)
{
	assert(arr->compare);
	uint32_t i = 0;
	while(i < arr->size && arr->compare(arr->values[i], val)) i++;
	if(arr->size < arr->max)
	{
		if(i == arr->size)
		{
			arr->values[arr->size++] = val;
		}
		else 
		{
			cast_t tmp = arr->values[i];
			arr->values[i] = val;
			while (i < arr->size)
			{
				i++;
				cast_t tmp2 = arr->values[i];
				arr->values[i] = tmp;
				tmp = tmp2;
			}
			arr->size++;
		}
		return 0;
	}
	return 1;
}
cast_t lookup_ordered_array(uint32_t i, ordered_array_t *arr)
{
	assert(i < arr->size);
	return arr->values[i];
}
void remove_ordered_array(uint32_t i, ordered_array_t *arr)
{
	while(i < arr->size)
	{
		arr->values[i] = arr->values[++i];
	}
	if(i == arr->size) arr->size--;
}