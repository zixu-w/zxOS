#include <kernel/memory.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

static multiboot_memory_map_t* mmap;

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
  const mem_addr_t paged_mmap_addr = (mem_addr_t) (mbi->mmap_addr+KERNEL_BASE);
  const uint32_t mmap_length = (uint32_t) mbi->mmap_length;
  mmap = (multiboot_memory_map_t*) paged_mmap_addr;
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

mem_addr_t get_phys_addr(mem_addr_t virt_addr) {
  size_t page_dir_index = virt_addr >> 22;
  pde_t pde = ((pde_t*) PAGE_DIR_ADDR)[page_dir_index];
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Getting physical address for 0x%08x.\n", virt_addr);
  printf("    page_dir_index = %d\n", page_dir_index);
  printf("    page_dir_entry = 0x%08x\n", pde);
#endif
  if (pde & PDE_PGSZ) {  // 4MB page
#ifdef _KERNEL_DEBUG
  printf("    (4M page) phys_addr = 0x%08x\n",
    (pde & ~0x003FFFFF) | (virt_addr & 0x003FFFFF));
#endif
    return (pde & ~0x003FFFFF) | (virt_addr & 0x003FFFFF);
  } else {  // 4KB page
    pte_t* page_table = (pte_t*) ((pde & ~0x0FFF) + KERNEL_BASE);
    size_t page_tab_index = (virt_addr >> 12) & 0x03FF;
    pte_t pte = page_table[page_tab_index];
#ifdef _KERNEL_DEBUG
  printf("    (4K page) page_table     = 0x%08x\n", (mem_addr_t) page_table);
  printf("              page_tab_index = %d\n", page_tab_index);
  printf("              page_tab_entry = 0x%08x\n", pte);
  printf("              phys_addr      = 0x%08x\n",
    (pte & ~0x0FFF) | (virt_addr & 0x0FFF));
#endif
    return (pte & ~0x0FFF) | (virt_addr & 0x0FFF);
  }
}

void map_page(mem_addr_t phys_addr, mem_addr_t virt_addr, uint16_t flags) {
  // TODO: 4K align addresses.
  if (flags & MAP_PGSZ) {  // Map 4M page.
    size_t page_dir_index = virt_addr >> 22;
    pde_t pde = 0;
    pde |= phys_addr & ~0x003FFFFF;
    pde |= flags & 0x1FFF;
    ((pde_t*) PAGE_DIR_ADDR)[page_dir_index] = pde | PDE_PRST;
    flush_tlb(phys_addr);
  } else {  // Map 4K page.
    // TODO
  }
}
