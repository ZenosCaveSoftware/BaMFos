#ifndef _KERNEL_ORD_H
#define _KERNEL_ORD_H

#if !defined(__cpluscplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

typedef void * cast_t;

typedef int8_t (*binary_comperator_t)(cast_t, cast_t);

typedef struct ordered_array_struct
{
	cast_t *values;
	uint32_t size;
	uint32_t max;
	binary_comperator_t compare;
}__attribute__((packed)) ordered_array_t;

int8_t lessthan_comperator(cast_t a, cast_t b);

ordered_array_t create_ordered_array(uint32_t max, binary_comperator_t compare);
ordered_array_t place_ordered_array(uintptr_t addr, uint32_t max, binary_comperator_t compare);
void destroy_ordered_array(ordered_array_t *arr);
uint8_t insert_ordered_array(cast_t val, ordered_array_t *arr);
cast_t lookup_ordered_array(uint32_t i, ordered_array_t *arr);
void remove_ordered_array(uint32_t i, ordered_array_t *arr);

#endif