#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stdint.h>
#include <kernel/multiboot.h>

#define NUM_PDE 1024
#define NUM_PTE 1024

typedef uint32_t mem_addr_t;
typedef uint32_t pde_t;
typedef uint32_t pte_t;

void init_mem(multiboot_info_t*);

#endif
