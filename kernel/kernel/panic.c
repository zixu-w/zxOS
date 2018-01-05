#include <kernel/kernel.h>
#include <kernel/tty.h>

__attribute__((__noreturn__))
void panic(const char* s) {
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
  printf("[ERROR] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Kernel panic: %s\n", s);
  halt();
  __builtin_unreachable();
}
