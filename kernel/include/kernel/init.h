#ifndef _KERNEL_INIT_H
#define _KERNEL_INIT_H

#include <kernel/multiboot.h>

#if defined(__cplusplus)
extern "C"
#else
extern
#endif
void *text_section_start,
     *text_section_end,
     *rodata_section_start,
     *rodata_section_end,
     *data_section_start,
     *data_section_end,
     *bss_section_start,
     *bss_section_end,
     *kernel_end;

void init_cpu(void);
void init_mem(multiboot_info_t*);

#endif
