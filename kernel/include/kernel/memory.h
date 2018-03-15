#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stdint.h>
#include <kernel/multiboot.h>

#define NULL ((void*) 0)

#define MEM_BLOCK_SIZE      4096            // 4K memory block
#define MEM_BLOCK_ALIGN     MEM_BLOCK_SIZE  // align at block size

#define KERNEL_BASE 0xC0000000

typedef uint32_t mem_addr_t;

static inline mem_addr_t block_align(mem_addr_t addr) {
  return (addr & -MEM_BLOCK_ALIGN) + MEM_BLOCK_ALIGN;
}

#if defined(__cplusplus)
extern "C"
#else
extern
#endif
void *kernel_virt_start,
     *kernel_phys_start,
     *kernel_virt_end,
     *kernel_phys_end;

void init_mem(multiboot_info_t*);

#endif
