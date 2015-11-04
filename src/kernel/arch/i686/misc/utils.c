#include <stddef.h>
#include <stdint.h>
#include <kernel/utils.h>

void outb(uint16_t port, uint8_t val)
{
	__asm__ __volatile__ ("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

void outw(uint16_t port, uint16_t val)
{
	__asm__ __volatile__ ("outw %w0, %w1" : : "a"(val), "Nd"(port));
}

void outl(uint16_t port, uint32_t val)
{
	__asm__ __volatile__ ("outl %0, %w1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ __volatile__ ("inb %w1, %b0" : "=a" (ret) : "dN" (port));
	return ret;
}

uint16_t inw(uint16_t port)
{
	uint16_t ret;
	__asm__ __volatile__ ("inw %w1, %w0" : "=a" (ret) : "dN" (port));
	return ret;
}

uint32_t inl(uint16_t port)
{
	uint32_t ret;
	__asm__ __volatile__ ("inl %w1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void io_wait()
{
	__asm__ __volatile__ ( "outb %%al, $0x80" : : "a"(0) );
}

uint8_t one_count(uint32_t value)
{
	int cnt = 0;
	while(value)
	{
		cnt += value & 0x1;
		value >>= 1;
	}
	return cnt;
}