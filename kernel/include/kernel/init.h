#ifndef _KERNEL_INIT_H
#define _KERNEL_INIT_H

#if defined(__cplusplus)
extern "C"
#else
extern
#endif
void *kernel_start,
     *kernel_end;

void init_cpu(void);

#endif
