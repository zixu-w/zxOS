#include "kernel/kernel.h"

__attribute__((__noreturn__))
void halt(void) {
  while (1)
    __asm__("hlt");
  __builtin_unreachable();
}
