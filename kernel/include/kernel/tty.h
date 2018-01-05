#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/vga.h>

void terminal_initialize(void);
void terminal_clear(void);
void terminal_scroll_up(size_t);
void terminal_setcolor(uint8_t);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

#endif
