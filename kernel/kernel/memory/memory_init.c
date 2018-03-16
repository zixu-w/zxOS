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
  printf("    mem_lower = %uKB, mem_upper = %uKB\n",
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
  const virt_addr_t paged_mmap_addr = (virt_addr_t)(mbi->mmap_addr+KERNEL_BASE);
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
        (phys_addr_t) (mmap_entry->addr & 0xFFFFFFFF)
      );
#endif
      phys_mem_init_region(
        (phys_addr_t) (mmap_entry->addr & 0xFFFFFFFF),
        (size_t) (mmap_entry->len & 0xFFFFFFFF)
      );
    }
  }

  phys_mem_deinit_region(
    (phys_addr_t) 0x0,
    (size_t)(&kernel_phys_end + phys_mem_get_max_blocks() / MEM_BLOCKS_PER_BYTE)
  );

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing virtual memory manager.\n");
#endif

  init_virt_mem_mngr();

#ifdef _KERNEL_DEBUG
  printf("    Page directory reinitialized at 0x%08x (VIRT).\n",
         virt_mem_get_page_dir());
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Memory environments initialized.\n");
  virt_mem_map_page(
    (phys_addr_t) phys_mem_alloc_block(),
    (virt_addr_t) 0x00001000,
    PTE_RW
  );
  printf("page_dir[0]: 0x%08x\n", virt_mem_get_page_dir()->entries[0]);
  int* test = (int*) 0x00001000;
  *test = 12345;
  printf("%d\n", *test);
#endif
}
