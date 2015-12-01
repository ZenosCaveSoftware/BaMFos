#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		putchar((int) ((const unsigned char*) data)[i]);
}

static void printi(const int data) {
	int tmp = data;
	int val;
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
			print(&c, sizeof(c));
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
			char *p;
			printi(d);
		}
		else
		{
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}
