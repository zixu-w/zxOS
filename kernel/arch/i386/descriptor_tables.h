#ifndef _ARCH_I386_DT_H
#define _ARCH_I386_DT_H

#include <stdint.h>

struct gdt_ptr_struct {
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

struct gdt_entry_struct {
  uint16_t limit_low_16;
  uint16_t base_low_16;
  uint8_t  base_mid_8;
  uint8_t  access;
  uint8_t  granularity;
  uint8_t  base_high_8;
}__attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

void init_descriptor_tables(void);

#endif
