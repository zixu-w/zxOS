/* Surely you will remove the processor conditionals and this comment
   appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#define _VGA_WIDTH  80
#define _VGA_HEIGHT 25

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static const size_t VGA_WIDTH = _VGA_WIDTH;
static const size_t VGA_HEIGHT = _VGA_HEIGHT;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
uint16_t  render_buffer[_VGA_WIDTH * _VGA_HEIGHT];
uint16_t  offscreen_up_buffer[_VGA_WIDTH * _VGA_HEIGHT];
uint16_t  offscreen_down_buffer[_VGA_WIDTH * _VGA_HEIGHT];

void terminal_update(size_t start_x, size_t start_y,
                     size_t end_x,   size_t end_y) {
  for (size_t y = start_y; y < VGA_HEIGHT; y++) {
    for (size_t x = start_x; x < VGA_WIDTH; x++) {
      if (y > end_y || (x == end_x && y == end_y))
        return;
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = render_buffer[index];
    }
  }
}

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			render_buffer[index] = vga_entry(' ', terminal_color);
			offscreen_up_buffer[index] = vga_entry(' ', terminal_color);
			offscreen_down_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
  terminal_update(0, 0, VGA_WIDTH, VGA_HEIGHT);
}

void terminal_scroll_up(size_t offset_row) {
  for (size_t y = offset_row; y < VGA_HEIGHT + offset_row; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      const size_t render_y = y - offset_row;
      const size_t render_index = render_y * VGA_WIDTH + x;
      if (y <= terminal_row) {
        const size_t up_y = VGA_HEIGHT - y;
        const size_t up_index = up_y * VGA_WIDTH + x;
        // TODO: scroll up offscreen_up_buffer.
        offscreen_up_buffer[up_index] = render_buffer[render_index];
      }
      if (y < VGA_HEIGHT)
        render_buffer[render_index] = terminal_buffer[index];
      else {
        const size_t down_y = y - VGA_HEIGHT;
        const size_t down_index = down_y * VGA_WIDTH + x;
        // TODO: scroll up offscreen_down_buffer.
        render_buffer[render_index] = offscreen_down_buffer[down_index];
      }
    }
  }
  terminal_update(0, 0, VGA_WIDTH, VGA_HEIGHT);
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	render_buffer[index] = vga_entry(c, color);
  terminal_update(x, y, x + 1, y);
}

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) {
      --terminal_row;
      terminal_scroll_up(1);
    }
  } else {
  	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  	if (++terminal_column == VGA_WIDTH) {
  		terminal_column = 0;
  		if (++terminal_row == VGA_HEIGHT) {
        --terminal_row;
        terminal_scroll_up(1);
      }
  	}
  }
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();

	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\n");
	terminal_writestring("Hello, newline works!\n");
	terminal_writestring(
    "This is a test trying to write across multiple lines to test if lines "
    "are correctly wrapped.\n"
  );
  terminal_writestring(
    "This is a test trying to write across multiple lines to test if lines "
    "are correctly wrapped.\n"
  );
  terminal_writestring(
    "This\nis\na\ntest\ntrying\nto\nwrite\nacross\nthe\nwhole\nscreen\nto\n"
    "test\nif\nup\nscrolling\nis\nworking\nor\nnot.\n"
  );
}
