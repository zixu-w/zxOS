#include <stdbool.h>

#include <kernel/virt_mem_mngr.h>
#include <kernel/phys_mem_mngr.h>

#undef _KERNEL_DEBUG

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

static inline size_t page_dir_index(mem_addr_t virt_addr) {
  return virt_addr >> 22;
}

static inline size_t page_tab_index(mem_addr_t virt_addr) {
  return (virt_addr >> 12) & 0x03FF;
}

static inline void pte_set_frame(pte_t* pte, mem_addr_t phys_addr) {
  *pte |= phys_addr & ~0x0FFF;
}

static inline mem_addr_t pte_get_frame(pte_t pte) {
  return pte & ~0x0FFF;
}

static inline void pte_add_flag(pte_t* pte, uint32_t flag) {
  *pte |= flag;
}

static inline void pte_del_flag(pte_t* pte, uint32_t flag) {
  *pte &= ~flag;
}

static inline bool pte_test_flag(pte_t pte, uint32_t flag) {
  return pte & flag;
}

bool virt_mem_alloc_page(pte_t* pte) {
  void* p = phys_mem_alloc_block();
  
  if (!p)
    return false;

  pte_set_frame(pte, (mem_addr_t) p);
  pte_add_flag(pte, PTE_PRST | PTE_RW);

  return true;
}

mem_addr_t get_phys_addr(mem_addr_t virt_addr) {
  size_t i = page_dir_index(virt_addr);
  pde_t pde = ((pde_t*) PAGE_DIR_ADDR)[i];
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Getting physical address for 0x%08x.\n", virt_addr);
  printf("    page_dir_index = %d\n", i);
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
    size_t j = page_tab_index(virt_addr);
    pte_t pte = page_table[j];
#ifdef _KERNEL_DEBUG
  printf("    (4K page) page_table     = 0x%08x\n", (mem_addr_t) page_table);
  printf("              page_tab_index = %d\n", j);
  printf("              page_tab_entry = 0x%08x\n", pte);
  printf("              phys_addr      = 0x%08x\n",
    pte_get_frame(pte) | (virt_addr & 0x0FFF));
#endif
    return pte_get_frame(pte) | (virt_addr & 0x0FFF);
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
