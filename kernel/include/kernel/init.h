#ifndef _KERNEL_INIT_H
#define _KERNEL_INIT_H

#include <kernel/multiboot.h>

void init_cpu(void);
void init_mem(multiboot_info_t*);

#endif
