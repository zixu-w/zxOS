#include <stdint.h>
#include <stddef.h>

#ifdef _KERNEL_DEBUG
#include <stdio.h>
#endif

#include "descriptor_tables.h"

extern void gdt_flush(uint32_t);

static void init_gdt(void);
static void gdt_set_gate(size_t, uint32_t, uint32_t, uint8_t, uint8_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;

void init_descriptor_tables(void) {
#ifdef _KERNEL_DEBUG
  printf("[INFO] Initializing descriptor tables.\n");
#endif
  init_gdt();
#ifdef _KERNEL_DEBUG
  printf("[INFO] Descriptor tables initialized.\n");
#endif
}

static void init_gdt(void) {
#ifdef _KERNEL_DEBUG
  printf("[INFO] Initializing GDT.\n");
#endif
  gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
  gdt_ptr.base  = (uint32_t) &gdt_entries;

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
#ifdef _KERNEL_DEBUG
  printf("[INFO] Flushing initialized GDT.\n");
#endif
  gdt_flush((uint32_t) &gdt_ptr);
#ifdef _KERNEL_DEBUG
  printf("[INFO] GDT flushed.\n");
#endif
}

static void gdt_set_gate(size_t num,
                         uint32_t base,
                         uint32_t limit,
                         uint8_t access,
                         uint8_t granularity) {
  gdt_entries[num].base_low_16 = (base & 0xFFFF);
  gdt_entries[num].base_mid_8  = (base >> 16) & 0xFF;
  gdt_entries[num].base_high_8 = (base >> 24) & 0xFF;

  gdt_entries[num].limit_low_16 = (limit & 0xFFFF);
  gdt_entries[num].granularity  = (limit >> 16) & 0x0F;

  gdt_entries[num].granularity |= granularity & 0xF0;
  gdt_entries[num].access       = access;
}
