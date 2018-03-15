#include <kernel/phys_mem_mngr.h>
#include <kernel/virt_mem_mngr.h>

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
  uint32_t mem_size = (uint32_t) mbi->mem_lower + (uint32_t) mbi->mem_upper;
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing physical memory manager with mem_size = %uKB.\n",
         mem_size);
#endif
  init_phys_mem_mngr(mem_size);
  const mem_addr_t paged_mmap_addr = (mem_addr_t) (mbi->mmap_addr+KERNEL_BASE);
  const uint32_t mmap_length = (uint32_t) mbi->mmap_length;
  multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) paged_mmap_addr;

  for (
    multiboot_memory_map_t* mmap_entry = mmap;
    (uint32_t) mmap_entry < paged_mmap_addr + mmap_length;
    mmap_entry = (multiboot_memory_map_t*) (
      (uint32_t) mmap_entry + mmap_entry->size + sizeof(mmap_entry->size)
    )
  ) {
    if ((uint32_t) mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
#ifdef _KERNEL_DEBUG
      printf(
        "    %uKB region starting at 0x%08x available.\n",
        (size_t) ((mmap_entry->len & 0xFFFFFFFF) / 1024),
        (mem_addr_t) (mmap_entry->addr & 0xFFFFFFFF)
      );
#endif
      phys_mem_init_region(
        (mem_addr_t) (mmap_entry->addr & 0xFFFFFFFF),
        (size_t) (mmap_entry->len & 0xFFFFFFFF)
      );
    }
  }

  phys_mem_deinit_region((mem_addr_t) 0x0, (size_t) (&kernel_phys_end));

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Memory environments initialized.\n");
  // printf("Page directory at 0x%08x (PHY).\n", get_phys_addr(PAGE_DIR_ADDR));
  // void* palloc_test = phys_mem_alloc_block();
  // printf("palloc_test:  0x%08x\n", palloc_test);
  // void* palloc_test2 = phys_mem_alloc_block();
  // printf("palloc_test2: 0x%08x\n", palloc_test2);
  // phys_mem_free_block(palloc_test2);
  // void* palloc_test3 = phys_mem_alloc_blocks(2);
  // printf("palloc_test3: 0x%08x\n", palloc_test3);
  // void* palloc_test4 = phys_mem_alloc_block();
  // printf("palloc_test4: 0x%08x\n", palloc_test4);
#endif
}
