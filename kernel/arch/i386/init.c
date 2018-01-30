#include <stdint.h>
#include <string.h>

#include <kernel/init.h>

#include "descriptor_tables.h"

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

void init_cpu(void) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing CPU environments.\n");
#endif
  init_descriptor_tables();
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("CPU environments initialized.\n");
#endif
}

void init_mem(multiboot_info_t* mbi) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing memory environments.\n");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Detecting memory.\n");
  printf("mem_lower = %uKB, mem_upper = %uKB\n",
         (uint32_t) mbi->mem_lower, (uint32_t) mbi->mem_upper);
#endif
  // TODO: initialize memory related environments: paging, heap, etc.
  multiboot_memory_map_t* mmap;
  const uint32_t paged_mmap_addr = (uint32_t) (mbi->mmap_addr + 0xC0000000);
  const uint32_t mmap_length = (uint32_t) mbi->mmap_length;
  mmap = (multiboot_memory_map_t*) paged_mmap_addr;
#ifdef _KERNEL_DEBUG
  printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
          paged_mmap_addr, mmap_length);
#endif
  for ( ; (uint32_t) mmap < paged_mmap_addr + mmap_length;
    mmap = (multiboot_memory_map_t*) (
      (uint32_t) mmap + mmap->size + sizeof(mmap->size)
    )
  ) {
    //TODO
#ifdef _KERNEL_DEBUG
  printf(
    "    size = 0x%x, base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x\n",
    (uint32_t) mmap->size,
    (uint32_t) ((mmap->addr + 0xC0000000) >> 32),
    (uint32_t) ((mmap->addr + 0xC0000000) & 0xFFFFFFFF),
    (uint32_t) (mmap->len >> 32),
    (uint32_t) (mmap->len & 0xFFFFFFFF),
    (uint32_t) mmap->type
  );
#endif
  }
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Memory environments initialized.\n");
#endif
}
