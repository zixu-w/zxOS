#include <stdbool.h>

#include <kernel/kernel.h>
#include <kernel/virt_mem_mngr.h>
#include <kernel/phys_mem_mngr.h>

#undef _KERNEL_DEBUG

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

static page_dir_t* current_page_dir = NULL;
static phys_addr_t current_pdbr     = 0x0;

static inline void enable_paging() {
  __asm__ __volatile__ (
    "movl %cr0, %ecx\n\t"
    "orl  $0x80000000, %ecx\n\t"
    "movl %ecx, %cr0"
  );
}

static inline void load_pdbr(phys_addr_t page_dir_addr) {
  __asm__ __volatile__ (
    "movl %0, %%ecx\n\t"
    "movl %%ecx, %%cr3"
    :: "r" (page_dir_addr)
  );
}

static inline phys_addr_t get_pdbr() {
  uint32_t pdbr;
  __asm__ __volatile__ (
    "movl %%cr3, %%ecx\n\t"
    "movl %%ecx, %0"
    : "=r" (pdbr)
  );
  return (phys_addr_t) pdbr;
}

static inline void switch_page_dir(page_dir_t* page_dir) {
  current_page_dir = page_dir;
  current_pdbr     = (phys_addr_t) &(page_dir->entries) - KERNEL_BASE;
  load_pdbr(current_pdbr);
}

static inline void flush_tlb(virt_addr_t virt_addr) {
  __asm__ __volatile__ ("invlpg (%0)" :: "r" (virt_addr) : "memory");
}

static inline size_t page_dir_index(virt_addr_t virt_addr) {
  return virt_addr >> 22;
}

static inline size_t page_tab_index(virt_addr_t virt_addr) {
  return (virt_addr >> 12) & 0x03FF;
}

static inline void pte_set_frame(pte_t* pte, phys_addr_t phys_addr) {
  *pte |= phys_addr & ~0x0FFF;
}

static inline void pde_set_frame(pde_t* pde, phys_addr_t phys_addr) {
  *pde |= phys_addr & ~0x0FFF;
}

static inline phys_addr_t pte_get_frame(pte_t pte) {
  return pte & ~0x0FFF;
}

static inline phys_addr_t pde_get_frame(pde_t pde) {
  return pde & ~0x0FFF;
}

static inline void pte_add_flag(pte_t* pte, uint32_t flag) {
  *pte |= flag;
}

static inline void pde_add_flag(pde_t* pde, uint32_t flag) {
  *pde |= flag;
}

static inline void pte_del_flag(pte_t* pte, uint32_t flag) {
  *pte &= ~flag;
}

static inline void pde_del_flag(pde_t* pde, uint32_t flag) {
  *pde &= ~flag;
}

static inline bool pte_test_flag(pte_t pte, uint32_t flag) {
  return pte & flag;
}

static inline bool pde_test_flag(pde_t pde, uint32_t flag) {
  return pde & flag;
}

static inline pte_t* page_tab_lookup(page_tab_t* pt, virt_addr_t virt_addr) {
  if (pt)
    return &(pt->entries[page_tab_index(virt_addr)]);
  return NULL;
}

static inline pde_t* page_dir_lookup(page_dir_t* pd, virt_addr_t virt_addr) {
  if (pd)
    return &(pd->entries[page_dir_index(virt_addr)]);
  return NULL;
}

page_dir_t* virt_mem_get_page_dir() {
  return current_page_dir;
}

bool virt_mem_alloc_page(pte_t* pte) {
  void* p = phys_mem_alloc_block() + KERNEL_BASE;

  if (!p)
    return false;

  pte_set_frame(pte, (phys_addr_t) (p - KERNEL_BASE));
  pte_add_flag(pte, PTE_PRST);

  return true;
}

void virt_mem_free_page(pte_t* pte) {
  void* p = (void*) (pte_get_frame(*pte) + KERNEL_BASE);

  if (p)
    phys_mem_free_block(p - KERNEL_BASE);

  pte_del_flag(pte, PTE_PRST);
}

bool virt_mem_map_page(
  phys_addr_t phys_addr, virt_addr_t virt_addr, uint32_t flags) {
  // TODO: 4K align addresses?
  page_dir_t* page_dir = virt_mem_get_page_dir();
  pde_t* pde = &(page_dir->entries[page_dir_index(virt_addr)]);

  if (flags & MAP_PGSZ) {  // Map 4M page.
    *pde |= phys_addr & ~0x003FFFFF;
    *pde |= (flags | PDE_PRST) & 0x1FFF;
    flush_tlb(virt_addr);
  } else {  // Map 4K page.
    if (!pde_test_flag(*pde, PDE_PRST)) {  // page table not present
      page_tab_t* page_tab =
        (page_tab_t*) (phys_mem_alloc_block() + KERNEL_BASE);

      if (!page_tab)
        return false;

      memset(page_tab, 0, sizeof(page_tab_t));

      pde_set_frame(pde, (phys_addr_t) (page_tab - KERNEL_BASE));
      pde_add_flag(pde, PDE_PRST | PDE_RW);
    }

    page_tab_t* page_tab = (page_tab_t*) (pde_get_frame(*pde) + KERNEL_BASE);
    pte_t* pte = &(page_tab->entries[page_tab_index(virt_addr)]);

    pte_set_frame(pte, phys_addr);
    pte_add_flag(pte, flags | PTE_PRST);
  }

  return true;
}

void init_virt_mem_mngr() {
  page_dir_t* page_dir = (page_dir_t*) (phys_mem_alloc_block() + KERNEL_BASE);

  if (!page_dir)
    panic("Cannot allocate memory for page directory!\n");

  memset(page_dir, 0, sizeof(page_dir_t));

  page_dir->entries[page_dir_index(KERNEL_BASE)] |= 0x00000083;
  switch_page_dir(page_dir);
  enable_paging();
}
