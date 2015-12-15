/* ===========================================================================
 * BamfOS 0.2.4
 * 
 *Please refer to LICENSE for copyright information
 * ===========================================================================
 */

/* ===========================================================================
 * BamfOS 0.2.4
 *
 *              BamfOS: A hobby OS designed for development
 *
 *              Kernel: The main kernel
 *
 *      File    : tty.c
 *      Purpose : basic console output
 *
 *      Notes   : Only works with basic vga 80x25 console as of now
 *      Author  : Luke Smith
 *      Changed : 09-12-2015::16:22 by Luke Smith
 * ===========================================================================
 */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * STANDARD INCLUDES
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * KERNEL INCLUDES
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <io/vga.h>
#include <io/tty.h>

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * GLOBAL VARIABLES
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * LOCAL FUNCTION DECLARATIONS
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

void terminal_scroll();
void terminal_foreground(enum vga_color fg);
void terminal_background(enum vga_color bg);
uint8_t terminal_escape_sequence(const char *es);
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * FUNCTION DEFINITIONS
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*
 * ---------------------------------------------------------------------------
 *      Name   : initialize_terminal 
 *      Purpose: setup basic global variables and environment conditions
 *      Args   : 
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void initialize_terminal()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    for ( size_t index = 0; index < VGA_HEIGHT * VGA_WIDTH; index++ )
    {
        terminal_buffer[index] = make_vgaentry('\0', terminal_color);
    }
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_setcolor 
 *      Purpose: change terminal color
 *      Args   : uint8_t color
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_putentryat 
 *      Purpose: insert a character and color into the buffer at the given
 *               offset (x and y)
 *      Args   : char c,
 *               uint8_t color,
 *               size_t x,
 *               size_t y
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = make_vgaentry(c, color);
}


/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_putchar
 *      Purpose: insert a char into buffer and move position
 *      Args   : char c
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
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

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_write
 *      Purpose: Write a string to the buffer.
 *      Args   : const char* data,
 *               size_t size
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
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
                terminal_putentryat(' ', 
                    make_color(COLOR_LIGHT_GREY, COLOR_BLACK),
                    terminal_column, terminal_row);
        }
        else if(data[i] == '\x1b')
        {
            i += terminal_escape_sequence(data + i);
        }
        else
        {
            terminal_putchar(data[i]);
        }
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_writestring
 *      Purpose: terminal_write() wrapper 
 *      Args   : const char *data
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_writehex
 *      Purpose: write an unsinged integer in hex of up to 32 bits without 
 *               leading zeroes
 *      Args   : const uint32_t data
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
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
        *--p = (tmp2 >= 0xa) 
            ? (char) ('a' + tmp2 - 0xa) 
            : (char) ('0' + tmp2);
        tmp = val;
    }
    terminal_write(p, strlen(p));
}


/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_scroll
 *      Purpose: scroll the buffer up one whole line
 *      Args   : 
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void terminal_scroll()
{
    int i = 0;
    for(;i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
        terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
    for(; i < VGA_WIDTH * VGA_HEIGHT; i++)
        terminal_buffer[i] = make_vgaentry(' ',
            make_color(COLOR_LIGHT_GREY, COLOR_BLACK));
    --terminal_row;
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_foreground
 *      Purpose: set the terminal foreground while keeping the background
 *      Args   : enum vga_color fg
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void terminal_foreground(enum vga_color fg)
{
    terminal_color = terminal_color & 0xf0 | fg;
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_background
 *      Purpose: set the terminal background while keeping the foreground
 *      Args   : enum vga_color bg
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
void terminal_background(enum vga_color bg)
{
    terminal_color = terminal_color & 0xf | (bg << 4);
}

/*
 * ---------------------------------------------------------------------------
 *      Name   : terminal_escape_sequence
 *      Purpose: edit terminal_vars based on an escape sequence.
 *      Args   : const char *es
 *      Returns: void
 * ---------------------------------------------------------------------------
 */
uint8_t terminal_escape_sequence(const char *es)
{
    const char *es2 = es + 1;
    if(*es2 == '\x1b') {
        terminal_writestring("^[^[");
        es2++;
        return es2 - es;
    }
    else if (*es2 == '[') es2++;
    if(es2[2] == 'm')
    {
        switch(*es2 - '0') {
            case(0):
                terminal_color 
                    = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
                break;
            case(3):
                if(es2[1] - '0' < 0x8)
                {
                    terminal_foreground(es2[1] - '0');
                }
                else
                {
                    terminal_writestring("^[3");
                    terminal_putchar(es2[1]);
                    terminal_putchar('m');
                }
                break;
            case(9):
                if(es2[1] - '0' < 0x8)
                {
                    terminal_foreground(es2[1] - '0' + 8);
                }
                else
                {
                    terminal_writestring("^[9");
                    terminal_putchar(es2[1]);
                    terminal_putchar('m');
                }
                break;
            case(4):
                if(es2[1] - '0' < 0x8)
                {
                    terminal_background(es2[1] - '0');
                }
                else
                {
                    terminal_writestring("^[4");
                    terminal_putchar(es2[1]);
                    terminal_putchar('m');
                }
                break;
            default:
                terminal_writestring("^[");
                terminal_putchar(*es2);
                terminal_putchar(es2[1]);
                terminal_putchar('m');
                break;
        }
        es2 += 2;  
        return es2 - es;
    }
    else if(*es2 == '1' && es2[1] == '0' && es2[3] == 'm')
    {
        if(es2[2] - '0' < 0x8)
        {
            terminal_background(es2[2] - '0' + 0x8);
        }
        else
        {
            terminal_writestring("^[10");
            terminal_putchar(es2[2]);
            terminal_putchar('m');
        }
        es2 += 3;
        return es2 - es;
    }
}