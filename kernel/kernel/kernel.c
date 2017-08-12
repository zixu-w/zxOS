#include <stdio.h>

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
#ifdef _KERNEL_DEBUG
  printf("[INFO] Entering kernal_main.\n");
#endif
  printf("Hello, kernel World!\n");
#ifdef _KERNEL_DEBUG
  printf("[INFO] Kernel exiting.\n");
#endif
}
