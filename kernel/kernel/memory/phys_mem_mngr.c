#include <kernel/phys_mem_mngr.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

static size_t    phys_mem_mngr_memory_size = 0;
static size_t    phys_mem_mngr_used_blocks = 0;
static size_t    phys_mem_mngr_max_blocks  = 0;
static uint32_t* phys_mem_mngr_memory_map  = (uint32_t*) (&kernel_virt_end);

size_t phys_mem_get_size() {
  return phys_mem_mngr_memory_size;
}

size_t phys_mem_get_max_blocks() {
  return phys_mem_mngr_max_blocks;
}

size_t phys_mem_get_used_blocks() {
  return phys_mem_mngr_used_blocks;
}

size_t phys_mem_get_free_blocks() {
  return phys_mem_mngr_max_blocks - phys_mem_mngr_used_blocks;
}

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
  for (i = 0; i < phys_mem_get_max_blocks() / 32; i++) {
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

int mmap_first_free_blocks(size_t size) {
  if (size <= 0)
    return -1;
  if (size == 1)
    return mmap_first_free_block();
  int start = -1;
  size_t length = 0;
  uint32_t i;
  for (i = 0; i < phys_mem_get_max_blocks() / 32; i++) {
    if (phys_mem_mngr_memory_map[i] != 0xFFFFFFFF) {
      uint32_t j;
      for (j = 0; j < 32; j++) {
        uint32_t bit = 1 << j;
        if (!(phys_mem_mngr_memory_map[i] & bit)) {
          if (start == -1)
            start = i * 32 + j;
          if (++length == size)
            return start;
        } else {
          start = -1;
          length = 0;
        }
      }
    }
  }
  return -1;
}

void init_phys_mem_mngr(size_t mem_size) {
  phys_mem_mngr_memory_size = mem_size;
  phys_mem_mngr_max_blocks  = (phys_mem_get_size() * 1024) / MEM_BLOCK_SIZE;
  phys_mem_mngr_used_blocks = phys_mem_mngr_max_blocks;

  memset(
    phys_mem_mngr_memory_map,
    0xF,
    phys_mem_get_max_blocks() / MEM_BLOCKS_PER_BYTE
  );
}

void phys_mem_init_region(mem_addr_t base, size_t size) {
  int    frame  = base / MEM_BLOCK_SIZE;
  size_t blocks = size / MEM_BLOCK_SIZE;

  for (; blocks > 0; blocks--) {
    mmap_unset(frame++);
    phys_mem_mngr_used_blocks--;
  }

  mmap_set(0);
}

void phys_mem_deinit_region(mem_addr_t base, size_t size) {
  int    frame  = base / MEM_BLOCK_SIZE;
  size_t blocks = size / MEM_BLOCK_SIZE;

  for (; blocks > 0; blocks--) {
    mmap_set(frame++);
    phys_mem_mngr_used_blocks++;
  }
}

void* phys_mem_alloc_block() {
  if (phys_mem_get_free_blocks() <= 0)
    return NULL;

  int frame = mmap_first_free_block();

  if (frame == -1)
    return NULL;

  mmap_set(frame);

  mem_addr_t addr = frame * MEM_BLOCK_SIZE;
  phys_mem_mngr_used_blocks++;

  return (void*) addr;
}

void phys_mem_free_block(void* p) {
  mem_addr_t addr = (mem_addr_t) p;
  int frame = addr / MEM_BLOCK_SIZE;

  mmap_unset(frame);

  phys_mem_mngr_used_blocks--;
}

void* phys_mem_alloc_blocks(size_t size) {
  if (phys_mem_get_free_blocks() < size)
    return NULL;

  int frame = mmap_first_free_blocks(size);

  if (frame == -1)
    return NULL;

  size_t i;
  for (i = 0; i < size; i++)
    mmap_set(frame + i);

  mem_addr_t addr = frame * MEM_BLOCK_SIZE;
  phys_mem_mngr_used_blocks += size;

  return (void*) addr;
}

void phys_mem_free_blocks(void* p, size_t size) {
  mem_addr_t addr = (mem_addr_t) p;
  int frame = addr / MEM_BLOCK_SIZE;

  size_t i;
  for (i = 0; i < size; i++)
    mmap_unset(frame + i);

  phys_mem_mngr_used_blocks -= size;
}
