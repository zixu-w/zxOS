#ifndef _KERNEL_PHYS_MEM_MNGR_H
#define _KERNEL_PHYS_MEM_MNGR_H

#include <stddef.h>
#include <kernel/memory.h>

#define MEM_BLOCKS_PER_BYTE 8               // 1 bit for 1 block

size_t phys_mem_get_size();
size_t phys_mem_get_max_blocks();
size_t phys_mem_get_used_blocks();
size_t phys_mem_get_free_blocks();
void init_phys_mem_mngr(size_t);
void phys_mem_init_region(mem_addr_t, size_t);
void phys_mem_deinit_region(mem_addr_t, size_t);
void* phys_mem_alloc_block();
void phys_mem_free_block(void*);
void* phys_mem_alloc_blocks(size_t);
void phys_mem_free_blocks(void*, size_t);

#endif
