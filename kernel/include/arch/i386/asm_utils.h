#ifndef _ARCH_I386_ASM_UTILS_H
#define _ARCH_I386_ASM_UTILS_H

#include <stdint.h>

static inline uint32_t farpeekl(uint16_t sel, void* off) {
  uint32_t ret;
  __asm__(
    "push %%fs\n\t"
    "mov  %1, %%fs\n\t"
    "mov  %%fs:(%2), %0\n\t"
    "pop  %%fs"
    : "=r" (ret) : "g" (sel), "r" (off)
  );
  return ret;
}

static inline void farpokeb(uint16_t sel, void* off, uint8_t v) {
  __asm__(
    "push %%fs\n\t"
    "mov  %0, %%fs\n\t"
    "movb %2, %%fs\n\t"
    "pop  %%fs"
    :: "g" (sel), "r" (off), "r" (v)
  );
}

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ __volatile__ ("outb %0, %1":: "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ __volatile__ ("inb %1, %0": "=a" (ret) : "Nd" (port));
  return ret;
}

static inline void io_wait(void) {
  __asm__ __volatile__ ("outb %%al, $0x80":: "a" (0));
}

#endif
