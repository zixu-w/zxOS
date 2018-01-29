#include <kernel/init.h>
#include <kernel/tty.h>

#ifdef _KERNEL_DEBUG
#include <stdio.h>
#endif

#if defined(__cplusplus)
extern "C"
#endif
void kernel_init(multiboot_info_t* mbi) {
  terminal_initialize();
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing kernel.\n");
#endif
  init_cpu();
  init_mem(mbi);
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Kernel initialized.\n");
#endif
}
