#include <kernel/memory.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

static size_t    phys_mem_mngr_memory_size = 0;
static size_t    phys_mem_mngr_used_blocks = 0;
static size_t    phys_mem_mngr_max_blocks  = 0;
static uint32_t* phys_mem_mngr_memory_map  = 0;

static inline void mmap_set(uint32_t bit) {
  phys_mem_mngr_memory_map[bit / 32] |= (1 << (bit % 32));
}

static inline void mmap_unset(uint32_t bit) {
  phys_mem_mngr_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

static inline uint32_t mmap_test(uint32_t bit) {
  return phys_mem_mngr_memory_map[bit / 32] & (1 << (bit % 32));
}

int mmap_first_free_block() {
  uint32_t i;
  for (i = 0; i < phys_mem_mngr_max_blocks / 32; i++) {
    if (phys_mem_mngr_memory_map[i] != 0xFFFFFFFF) {
      uint32_t j;
      for (j = 0; j < 32; j++) {
        uint32_t bit = 1 << j;
        if (!(phys_mem_mngr_memory_map[i] & bit))
          return i * 32 + j;
      }
    }
  }
  return -1;
}

void init_phys_mem_mngr(size_t mem_size, mem_addr_t bitmap) {
  phys_mem_mngr_memory_size = mem_size;
  phys_mem_mngr_memory_map  = (uint32_t*) bitmap;
  phys_mem_mngr_max_blocks  = mem_size / MEM_BLOCK_SIZE;
}
