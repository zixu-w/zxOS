#include <kernel/init.h>

#if defined(__cplusplus)
extern "C"
#endif
void kernel_init(void) {
  // TODO: Set up GDT, IDT, memory management, paging, etc.
  init_cpu();
  init_mem();
}
