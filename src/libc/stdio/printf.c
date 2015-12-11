#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		putchar((int) ((const unsigned char*) data)[i]);
}

static void printi(const int32_t data) {
	int32_t tmp = data;
	int32_t val;
	char str[33];
	char *p = &str[sizeof(str) - 1];
	*p = '\0';
	val = tmp / 10;
	*--p = ('0' + (char) (tmp - val * 10));
	tmp = val;
	while(tmp != 0)
	{
		val = tmp / 10;
		*--p = ('0' + (char) (tmp - val * 10));
		tmp = val;
	}
	if(!strlen(p)) *--p = '0';
	print(p, strlen(p));
}

static void printx(const uint32_t data) {
	uint32_t tmp = data;
	uint32_t tmp2 = 0;
	uint32_t val = 0;
	char str[17];
	char *p = &str[sizeof(str) - 1];
	*p = '\0';
	val = tmp / 16;
	tmp2 = tmp - val * 16;
	*--p = (tmp2 >= 0xa) ? (char) ('a' + tmp2 - 0xa) : (char) ('0' + tmp2);
	tmp = val;
	while(tmp != 0)
	{
		val = tmp / 16;
		tmp2 = tmp - val * 16;
		*--p = (tmp2 >= 0xa) ? (char) ('a' + tmp2 - 0xa) : (char) ('0' + tmp2);
		tmp = val;
	}

	if(!strlen(p)) *--p = '0';

	print(p, strlen(p));
}

int printf(const char* restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;

	while ( *format != '\0' )
	{
		if ( *format != '%' )
		{
		print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if ( *(++format) == '%' )
			goto print_c;

		if ( rejected_bad_specifier )
		{
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if ( *format == 'c' )
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, 1);
		}
		else if ( *format == 's' )
		{
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else if ( *format == 'd' )
		{
			format++;
			int d = (int) va_arg(parameters, int);
			printi(d);
		}
		else if ( *format == 'x' )
		{
			format++;
			int d = (int) va_arg(parameters, int);
			printx(d);
		}
		else
		{
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}
