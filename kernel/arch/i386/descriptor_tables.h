#ifndef _ARCH_I386_DT_H
#define _ARCH_I386_DT_H

#include <stdint.h>

#define PIC1          0x20
#define PIC2          0xA0
#define PIC1_COMMAND  PIC1
#define PIC1_DATA     (PIC1 + 1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA     (PIC2 + 1)
#define PIC_EOI       0x20

#define ICW1_ICW4       0x01
#define ICW1_SINGLE     0x02
#define ICW1_INTERVAL4  0x04
#define ICW1_LEVEL      0x08
#define ICW1_INIT       0x10

#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM       0x10

struct gdt_ptr_struct {
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

struct idt_ptr_struct {
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

struct gdt_entry_struct {
  uint16_t limit_low_16;
  uint16_t base_low_16;
  uint8_t  base_mid_8;
  uint8_t  access;
  uint8_t  granularity;
  uint8_t  base_high_8;
}__attribute__((packed));

struct idt_entry_struct {
  uint16_t base_low_16;
  uint16_t selector;
  uint8_t  zero;
  uint8_t  flags;
  uint16_t base_high_16;
}__attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;
typedef struct gdt_entry_struct gdt_entry_t;
typedef struct idt_ptr_struct idt_ptr_t;
typedef struct idt_entry_struct idt_entry_t;

void init_descriptor_tables(void);

extern void isr0 (void);
extern void isr1 (void);
extern void isr2 (void);
extern void isr3 (void);
extern void isr4 (void);
extern void isr5 (void);
extern void isr6 (void);
extern void isr7 (void);
extern void isr8 (void);
extern void isr9 (void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0 (void);
extern void irq1 (void);
extern void irq2 (void);
extern void irq3 (void);
extern void irq4 (void);
extern void irq5 (void);
extern void irq6 (void);
extern void irq7 (void);
extern void irq8 (void);
extern void irq9 (void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

#endif
