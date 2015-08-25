#include <stdio.h>

#if defined(__is_bamf_kernel)
#include <kernel/tty.h>
#endif

int putchar(int ic)
{
#if defined(__is_bamf_kernel)
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: implement a write system call.
#endif
	return ic;
}
