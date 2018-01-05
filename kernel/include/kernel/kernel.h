#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arch/i386/asm_utils.h>

__attribute__((__noreturn__))
void halt(void);

__attribute__((__noreturn__))
void panic(const char*);

#endif
