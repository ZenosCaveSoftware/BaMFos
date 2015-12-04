#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

void panic(const char *message, const char *file, uint32_t line)
{
    __asm__ __volatile__ ("cli"); // Disable interrupts.
    printf("\n!!! %s !!!", message);
    for(;;);
}