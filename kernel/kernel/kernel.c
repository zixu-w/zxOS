#include <kernel/kernel.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#endif

void foo(const char* s) {
  char buffer[8];
  memcpy(buffer, s, 9);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Entering kernal_main.\n");
#endif
  printf("Hello, kernel World!\n");
  // foo("This is a test of SSP");
  // __asm__ __volatile__ ("int $0x3");
  halt();
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Kernel exiting.\n");
#endif
}
