#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>

static const size_t    VGA_WIDTH  = _VGA_WIDTH;
static const size_t    VGA_HEIGHT = _VGA_HEIGHT;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xC00B8000;

static size_t    terminal_row;
static size_t    terminal_column;
static uint8_t   terminal_color;
static uint16_t* terminal_buffer;
static uint16_t  render_buffer        [_VGA_WIDTH * _VGA_HEIGHT];
static uint16_t  offscreen_up_buffer  [_VGA_WIDTH * _VGA_HEIGHT];
static uint16_t  offscreen_down_buffer[_VGA_WIDTH * _VGA_HEIGHT];

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
	terminal_buffer = VGA_MEMORY;
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

void terminal_clear(void) {
  terminal_scroll_up(VGA_HEIGHT);
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
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
  	unsigned char uc = c;
  	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
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
