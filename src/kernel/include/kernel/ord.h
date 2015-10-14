#ifndef _KERNEL_ORD_H
#define _KERNEL_ORD_H 1

typedef void* proto_t;

typedef int8_t (*binary_comperator_t)(proto_t, proto_t);

typedef struct
{
	proto_t *values;
	uint32_t size;
	uint32_t max_size;
	binary_comperator_t comp;
} ordered_array_t;

int8_t statndard_order_comperator(proto_t, proto_t);

ordered_array_t create_ordered_array(uint32_t, binary_comperator_t);
ordered_array_t place_ordered_array(void *, uint32_t, binary_comperator_t);
void destroy_ordered_array(ordered_array_t *);
int insert_ordered_array(proto_t, ordered_array_t *array);
proto_t lookup_ordered_array(uint32_t, ordered_array_t *);
void remove_ordered_array(uint32_t, ordered_array_t *);

#endif