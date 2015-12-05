#include <stdio.h>

int puts(const char* string)
{
	if(string) return printf("%s\n", string);
	return printf("\n");
}
