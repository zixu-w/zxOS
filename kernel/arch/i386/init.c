#include <stdint.h>
#include <string.h>

#include <kernel/init.h>

#include "descriptor_tables.h"

void init_cpu(void) {
  init_descriptor_tables();
}

void init_mem(void) {
  // TODO: initialize memory related environments: paging, heap, etc.
}
