#ifndef _KERNEL_VIRT_MEM_MNGR_H
#define _KERNEL_VIRT_MEM_MNGR_H

#include <stddef.h>
#include <kernel/memory.h>

#define PAGE_DIR_ADDR 0xFFFFF000

#define NUM_PDE 1024
#define NUM_PTE 1024

#define PDE_PRST 0x1
#define PDE_RW   0x2
#define PDE_US   0x4
#define PDE_PWT  0x8
#define PDE_PCD  0x10
#define PDE_ACCS 0x20
#define PDE_DIRT 0x40
#define PDE_PGSZ 0x80
#define PDE_GL   0x100
#define PDE_PAT  0x200
#define PTE_PRST 0x1
#define PTE_RW   0x2
#define PTE_US   0x4
#define PTE_PWT  0x8
#define PTE_PCD  0x10
#define PTE_ACCS 0x20
#define PTE_DIRT 0x40
#define PTE_PAT  0x80
#define PTE_GL   0x100
#define MAP_PGSZ 0x8000

typedef uint32_t pde_t;
typedef uint32_t pte_t;

static inline void flush_tlb(mem_addr_t virt_addr) {
  __asm__ __volatile__ ("invlpg (%0)" :: "r" (virt_addr) : "memory");
}

mem_addr_t get_phys_addr(mem_addr_t);
void map_page(mem_addr_t, mem_addr_t, uint16_t);

#endif
