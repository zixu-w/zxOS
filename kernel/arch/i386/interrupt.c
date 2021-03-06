#include <stdio.h>

#include <kernel/kernel.h>
#include <arch/i386/asm_utils.h>
#include <kernel/interrupt.h>

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler) {
  interrupt_handlers[n] = handler;
}

void isr_handler(registers_t regs) {
  if (interrupt_handlers[regs.int_no] != 0)
    interrupt_handlers[regs.int_no](regs);
  else {
    printf("isr %d; eip: 0x%08x\n", regs.int_no, regs.eip);
    panic("Unhandled interrupt!");
  }
}

void irq_handler(registers_t regs) {
  if (regs.int_no >= 40)
    outb(0xA0, 0x20);
  outb(0x20, 0x20);
  if (interrupt_handlers[regs.int_no] != 0) {
    isr_t handler = interrupt_handlers[regs.int_no];
    handler(regs);
  }
}
