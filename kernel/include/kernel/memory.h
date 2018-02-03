#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/multiboot.h>

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

typedef uint32_t mem_addr_t;
typedef uint32_t pde_t;
typedef uint32_t pte_t;

struct avail_phys_mem_node {
  struct avail_phys_mem_node* prev;
  struct avail_phys_mem_node* next;
  size_t size;
};

typedef struct avail_phys_mem_node avail_phys_mem_node_t;

static inline void flush_tlb(mem_addr_t addr) {
  __asm__ __volatile__ ("invlpg (%0)" :: "r" (addr) : "memory");
}

void init_mem(multiboot_info_t*);
mem_addr_t get_phys_addr(mem_addr_t);
void map_page(mem_addr_t, mem_addr_t, uint16_t);

#endif
