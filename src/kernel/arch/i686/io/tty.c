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
	terminal_writestring("[TTY]   ... DONE\n");
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
			terminal_row = 0;
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
				terminal_row = 0;	
			}
		}
		else if(data[i] == '\t')
		{
			size_t i = 0;
			while(i++ < 4 - (terminal_column % 4))
				terminal_putchar(' ');
			
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

terminal_writehex(const uint32_t data)
{
	int tmp = data;
	int tmp2 = 0;
	int val = 0;
	char str[17];
	char *p = &str[sizeof(str) - 1];
	*p = '\0';
	val = tmp / 16;
	*--p = ('0' + (char) (tmp - val * 16));
	tmp = val;
	while(tmp != 0)
	{
		val = tmp / 16;
		tmp2 = tmp - val * 16;
		*--p = (tmp2 < 10) ? ('0' + (char) (tmp2)) : ('a' + (char) (tmp2 - 10));
		tmp = val;
	}
	terminal_write(p, strlen(p));
}