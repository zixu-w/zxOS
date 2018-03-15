#include <kernel/virt_mem_mngr.h>

#undef _KERNEL_DEBUG

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

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
    flush_tlb(virt_addr);
  } else {  // Map 4K page.
    // TODO
  }
}
