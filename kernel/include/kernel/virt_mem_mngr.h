#ifndef _KERNEL_VIRT_MEM_MNGR_H
#define _KERNEL_VIRT_MEM_MNGR_H

#include <stddef.h>
#include <stdbool.h>
#include <kernel/memory.h>

#define PD_VIRT_ADDR 0xFFFFF000

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

struct page_table {
  pte_t entries[NUM_PTE];
}__attribute__((aligned(4096)));

struct page_directory {
  pde_t entries[NUM_PDE];
}__attribute__((aligned(4096)));

typedef struct page_table     page_tab_t;
typedef struct page_directory page_dir_t;

void init_virt_mem_mngr();
page_dir_t* virt_mem_get_page_dir();
bool virt_mem_map_page(phys_addr_t, virt_addr_t, uint32_t);
phys_addr_t virt_to_phys(virt_addr_t);

#endif
