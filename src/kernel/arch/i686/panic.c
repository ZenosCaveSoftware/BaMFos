#include <stddef.h>
#include <stdint.h>
#include <kernel/panic.h>

extern void panic(const char *message, const char *file, uint32_t line)
{

    __asm__ __volatile__ ("cli"); // Disable interrupts.

    for(;;);
}