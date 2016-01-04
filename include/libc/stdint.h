#ifndef _STDINT_H
#define _STDINT_H 1

#include <sys/cdefs.h>

typedef signed char			int8_t;
typedef signed short int	int16_t;
typedef signed int			int32_t;

#if __WORDSIZE == 64
typedef signed long	int		int64_t;
typedef signed long int 	intptr_t;
#else
typedef signed long	int		int32_t;
typedef intptr_t			int32_t;
#endif

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;

#if __WORDSIZE == 64
typedef unsigned long int	uint64_t;
typedef unsigned long int	uintptr_t;
#else
typedef unsigned long int	int32_t;
typedef unsigned long int 	uintptr_t;
#endif


#endif