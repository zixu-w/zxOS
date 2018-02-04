#ifndef _KERNEL_INIT_H
#define _KERNEL_INIT_H

#if defined(__cplusplus)
extern "C"
#else
extern
#endif
void *kernel_virt_start,
     *kernel_phys_start,
     *kernel_virt_end,
     *kernel_phys_end;

void init_cpu(void);

#endif
