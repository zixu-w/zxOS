#include <kernel/init.h>
#include <kernel/tty.h>

#ifdef _KERNEL_DEBUG
#include <stdio.h>
#endif

#if defined(__cplusplus)
extern "C"
#endif
void kernel_init(void) {
  terminal_initialize();
#ifdef _KERNEL_DEBUG
  printf("[INFO] Initializing kernel.\n");
#endif
  init_cpu();
  init_mem();
#ifdef _KERNEL_DEBUG
  printf("[INFO] Kernel initialized.\n");
#endif
}
