#include <kernel/kernel.h>
#include <kernel/virt_mem_mngr.h>
#include <kernel/phys_mem_mngr.h>
#include <kernel/interrupt.h>

// #undef _KERNEL_DEBUG

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

static inline void switch_page_dir(page_dir_t* page_dir) {
  current_page_dir = page_dir;
  current_pdbr = pde_get_frame(page_dir->entries[page_dir_index(PD_VIRT_ADDR)]);
  load_pdbr(current_pdbr);
}

page_dir_t* virt_mem_get_page_dir() {
  return current_page_dir;
}

bool virt_mem_alloc_page(pte_t* pte) {
  void* phys_block = phys_mem_alloc_block();

  if (!phys_block)
    return false;

  pte_set_frame(pte, (phys_addr_t) phys_block);
  pte_add_flag(pte, PTE_PRST);

  return true;
}

void virt_mem_free_page(pte_t* pte) {
  phys_addr_t frame = pte_get_frame(*pte);

  if (frame)
    phys_mem_free_block((void*) frame);

  pte_del_flag(pte, PTE_PRST);
}

phys_addr_t virt_to_phys(virt_addr_t virt_addr) {
  page_tab_t* page_tab = (page_tab_t*) (0xFFC00000 + page_dir_index(virt_addr));
  pte_t* pte = page_tab_lookup(page_tab, virt_addr);
  return pte_get_frame(*pte);
}

bool virt_mem_map_page(
  phys_addr_t phys_addr, virt_addr_t virt_addr, uint32_t flags) {
  // TODO: 4K align addresses?
  page_dir_t* page_dir = virt_mem_get_page_dir();
  pde_t* pde = &(page_dir->entries[page_dir_index(virt_addr)]);

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Mapping 0x%08x (PHYS) to 0x%08x (VIRT).\n", phys_addr, virt_addr);
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("page_dir@ 0x%08x; pde@ 0x%08x.\n", page_dir, pde);
#endif

  if (flags & MAP_PGSZ) {  // Map 4M page.
    *pde |= phys_addr & ~0x003FFFFF;
    *pde |= (flags | PDE_PRST) & 0x1FFF;
    flush_tlb(virt_addr);
  } else {  // Map 4K page.
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Mapping 4K page.\n");
#endif
    page_tab_t* page_tab =
      (page_tab_t*) (0xFFC00000 + page_dir_index(virt_addr));

    if (!pde_test_flag(*pde, PDE_PRST)) {  // page table not present
      void* phys_block = phys_mem_alloc_block();

      if (!phys_block)
        return false;

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf(
    "Page table not presented, allocating at 0x%08x (PHYS).\n",
    (phys_addr_t) phys_block
  );
#endif

      pde_set_frame(pde, (phys_addr_t) phys_block);
      pde_add_flag(pde, PDE_PRST | PDE_RW);

      memset(page_tab, 0, sizeof(page_tab_t));

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("New page table initialized.\n");
#endif
    }

    pte_t* pte = &(page_tab->entries[page_tab_index(virt_addr)]);

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("page_tab@ 0x%08x; pte@ 0x%08x.\n", page_tab, pte);
#endif

    pte_set_frame(pte, phys_addr);
    pte_add_flag(pte, flags | PTE_PRST);
  }

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[virt_mem_map_page] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Page mapped.\n");
#endif

  return true;
}

void page_fault_handler(registers_t regs) {
  virt_addr_t faulting_address;
  __asm__ __volatile__ ("movl %%cr2, %0" : "=r" (faulting_address));

  int prst = regs.err_no & 0x1;
  int rw   = regs.err_no & 0x2;
  int usr  = regs.err_no & 0x4;
  int rsv  = regs.err_no & 0x8;

  printf(
    "Page fault at 0x%08x\n"
    "(present: %d, read-only: %d, user-mode: %d, reserved: %d)\n",
    faulting_address, prst, rw, usr, rsv
  );

  panic("Page fault!");
}

void init_virt_mem_mngr() {
  void* phys_block = phys_mem_alloc_block();

  if (!phys_block)
    panic("Cannot allocate memory for page directory!\n");

  page_dir_t* page_dir = (page_dir_t*) ((phys_addr_t) phys_block + KERNEL_BASE);

  memset(page_dir, 0, sizeof(page_dir_t));

  page_dir->entries[page_dir_index(KERNEL_BASE)] |= 0x00000083;
  page_dir->entries[page_dir_index(PD_VIRT_ADDR)] |=
    (phys_addr_t) phys_block + 0x00000003;

  register_interrupt_handler(14, page_fault_handler);

  switch_page_dir(page_dir);
  enable_paging();
}
