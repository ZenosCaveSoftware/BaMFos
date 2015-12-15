#ifndef _STDINT_H
#define _STDINT_H 1

typedef signed char			int8_t;
typedef signed short int	int16_t;
typedef signed int			int32_t;

#if __WORDSIZE == 64
typedef signed long			int64_t;
#endif

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;

#if __WORDSIZE == 64
typedef unsigned long int	uint64_t;
#endif

typedef signed int 			intptr_t;
typedef unsigned int 		uintptr_t;

#endif