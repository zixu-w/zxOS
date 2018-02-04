#include <kernel/memory.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

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
  const mem_addr_t paged_mmap_addr = (mem_addr_t) (mbi->mmap_addr+KERNEL_BASE);
  const uint32_t mmap_length = (uint32_t) mbi->mmap_length;
  multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) paged_mmap_addr;
#ifdef _KERNEL_DEBUG
  printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
          paged_mmap_addr, mmap_length);
#endif
  for (multiboot_memory_map_t* mmap_entry = mmap;
    (uint32_t) mmap_entry < paged_mmap_addr + mmap_length;
    mmap_entry = (multiboot_memory_map_t*) (
      (uint32_t) mmap_entry + mmap_entry->size + sizeof(mmap_entry->size)
    )
  ) {
#ifdef _KERNEL_DEBUG
  printf(
    "    size = 0x%x, base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x\n",
    (uint32_t) mmap_entry->size,
    (mem_addr_t) ((mmap_entry->addr + KERNEL_BASE) >> 32),
    (mem_addr_t) ((mmap_entry->addr + KERNEL_BASE) & 0xFFFFFFFF),
    (uint32_t) (mmap_entry->len >> 32),
    (uint32_t) (mmap_entry->len & 0xFFFFFFFF),
    (uint32_t) mmap_entry->type
  );
#endif
  }
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Memory environments initialized.\n");
  printf("Page directory at 0x%08x (PHY).\n", get_phys_addr(PAGE_DIR_ADDR));
#endif
}
