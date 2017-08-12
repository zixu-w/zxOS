#include <stdint.h>
#include <string.h>

#include <kernel/init.h>

#include "descriptor_tables.h"

#ifdef _KERNEL_DEBUG
#include <stdio.h>
#endif

void init_cpu(void) {
#ifdef _KERNEL_DEBUG
  printf("[INFO] Initializing CPU environments.\n");
#endif
  init_descriptor_tables();
#ifdef _KERNEL_DEBUG
  printf("[INFO] CPU environments initialized.\n");
#endif
}

void init_mem(void) {
#ifdef _KERNEL_DEBUG
  printf("[INFO] Initializing memory environments.\n");
#endif
  // TODO: initialize memory related environments: paging, heap, etc.
#ifdef _KERNEL_DEBUG
  printf("[INFO] Memory environments initialized.\n");
#endif
}
