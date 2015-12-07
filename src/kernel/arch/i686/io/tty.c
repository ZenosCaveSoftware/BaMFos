#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/vga.h>
#include <kernel/tty.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_scroll();

void initialize_terminal()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry('\0', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		if ( ++terminal_row == VGA_HEIGHT )
		{
			terminal_scroll();
		}
	}
}

void terminal_write(const char* data, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		if(data[i] == '\n')
		{
			terminal_column = 0;
			if(++terminal_row == VGA_HEIGHT)
			{
				terminal_scroll();
			}
		}
		else if(data[i] == '\t')
		{
			size_t i = 0;
			while(i++ < 4 - (terminal_column % 4))
				terminal_putchar(' ');
			
		}
		else if(data[i] == '\r')
		{
			
			while(terminal_column-- >= 0)
				terminal_putentryat(' ', make_color(COLOR_LIGHT_GREY, COLOR_BLACK),terminal_column, terminal_row);
		}
		else
		{
			terminal_putchar(data[i]);
		}
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}

void terminal_writehex(const uint32_t data)
{
	uint32_t tmp = data;
	uint32_t tmp2 = 0;
	int val = 0;
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
	terminal_write(p, strlen(p));
}

void terminal_scroll()
{
	int i = 0;
	for(;i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
		terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
	for(; i < VGA_WIDTH * VGA_HEIGHT; i++)
		terminal_buffer[i] = make_vgaentry(' ',  make_color(COLOR_LIGHT_GREY, COLOR_BLACK));
	--terminal_row;
}