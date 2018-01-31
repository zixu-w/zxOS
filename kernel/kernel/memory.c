#include <kernel/memory.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

static pde_t* page_directory;
static multiboot_memory_map_t* mmap;

void init_mem(multiboot_info_t* mbi) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing memory environments.\n");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Detecting memory.\n");
  printf("mem_lower = %uKB, mem_upper = %uKB\n",
         (uint32_t) mbi->mem_lower, (uint32_t) mbi->mem_upper);
#endif
  // TODO: initialize memory related environments: paging, heap, etc.
  const mem_addr_t paged_mmap_addr = (mem_addr_t) (mbi->mmap_addr + 0xC0000000);
  const uint32_t mmap_length = (uint32_t) mbi->mmap_length;
  mmap = (multiboot_memory_map_t*) paged_mmap_addr;
#ifdef _KERNEL_DEBUG
  printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
          paged_mmap_addr, mmap_length);
#endif
  for (multiboot_memory_map_t* mmap_entry = mmap;
    (uint32_t) mmap_entry < paged_mmap_addr + mmap_length;
    mmap_entry = (multiboot_memory_map_t*) (
      (uint32_t) mmap_entry + mmap_entry->size + sizeof(mmap_entry->size)
    )
  ) {
#ifdef _KERNEL_DEBUG
  printf(
    "    size = 0x%x, base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x\n",
    (uint32_t) mmap_entry->size,
    (mem_addr_t) ((mmap_entry->addr + 0xC0000000) >> 32),
    (mem_addr_t) ((mmap_entry->addr + 0xC0000000) & 0xFFFFFFFF),
    (uint32_t) (mmap_entry->len >> 32),
    (uint32_t) (mmap_entry->len & 0xFFFFFFFF),
    (uint32_t) mmap_entry->type
  );
#endif
  }
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Memory environments initialized.\n");
  mem_addr_t page_directory_addr;
  __asm__ __volatile__ ("mov %%cr3, %0" : "=r" (page_directory_addr));
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Page directory at 0x%08x (PHY).\n", page_directory_addr);
  page_directory = (pde_t*) (page_directory_addr + 0xC0000000);
  printf("page_directory[0]    = 0x%08x.\n", page_directory[0]);
  printf("page_directory[768]  = 0x%08x.\n", page_directory[768]);
  printf("page_directory[1023] = 0x%08x.\n", page_directory[NUM_PDE - 1]);
#endif
}
