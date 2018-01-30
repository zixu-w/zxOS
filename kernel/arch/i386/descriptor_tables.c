#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <kernel/kernel.h>

#ifdef _KERNEL_DEBUG
#include <kernel/tty.h>
#include <stdio.h>
#endif

#include "descriptor_tables.h"

extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);

static void init_gdt(void);
static void init_idt(void);
static void gdt_set_gate(size_t, uint32_t, uint32_t, uint8_t, uint8_t);
static void idt_set_gate(uint8_t, uint32_t, uint16_t, uint8_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void remap_PIC(int offset1, int offset2) {
  uint8_t a1 = inb(PIC1_DATA);
  uint8_t a2 = inb(PIC2_DATA);

  outb(0x20, 0x11);
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, offset1);
  io_wait();
  outb(PIC2_DATA, offset2);
  io_wait();
  outb(PIC1_DATA, 4);
  io_wait();
  outb(PIC2_DATA, 2);
  io_wait();

  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, a1);
  io_wait();
  outb(PIC2_DATA, a2);
  io_wait();
}

void init_descriptor_tables(void) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing descriptor tables.\n");
#endif
  init_gdt();
  init_idt();
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Descriptor tables initialized.\n");
#endif
}

static void init_gdt(void) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing GDT.\n");
#endif
  gdt_ptr.limit = sizeof(gdt_entry_t) * 5 - 1;
  gdt_ptr.base  = (uint32_t) &gdt_entries;

  memset(&gdt_entries, 0, sizeof(gdt_entry_t) * 5);

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Flushing initialized GDT at 0x%x.\n", &gdt_ptr);
#endif
  gdt_flush((uint32_t) &gdt_ptr);
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("GDT flushed.\n");
#endif
}

static void init_idt(void) {
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Initializing IDT.\n");
#endif
  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base  = (uint32_t) &idt_entries;

  memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

  idt_set_gate(0, (uint32_t) isr0, 0x08, 0x8E);
  idt_set_gate(1, (uint32_t) isr1, 0x08, 0x8E);
  idt_set_gate(2, (uint32_t) isr2, 0x08, 0x8E);
  idt_set_gate(3, (uint32_t) isr3, 0x08, 0x8E);
  idt_set_gate(4, (uint32_t) isr4, 0x08, 0x8E);
  idt_set_gate(5, (uint32_t) isr5, 0x08, 0x8E);
  idt_set_gate(6, (uint32_t) isr6, 0x08, 0x8E);
  idt_set_gate(7, (uint32_t) isr7, 0x08, 0x8E);
  idt_set_gate(8, (uint32_t) isr8, 0x08, 0x8E);
  idt_set_gate(9, (uint32_t) isr9, 0x08, 0x8E);
  idt_set_gate(10, (uint32_t) isr10, 0x08, 0x8E);
  idt_set_gate(11, (uint32_t) isr11, 0x08, 0x8E);
  idt_set_gate(12, (uint32_t) isr12, 0x08, 0x8E);
  idt_set_gate(13, (uint32_t) isr13, 0x08, 0x8E);
  idt_set_gate(14, (uint32_t) isr14, 0x08, 0x8E);
  idt_set_gate(15, (uint32_t) isr15, 0x08, 0x8E);
  idt_set_gate(16, (uint32_t) isr16, 0x08, 0x8E);
  idt_set_gate(17, (uint32_t) isr17, 0x08, 0x8E);
  idt_set_gate(18, (uint32_t) isr18, 0x08, 0x8E);
  idt_set_gate(19, (uint32_t) isr19, 0x08, 0x8E);
  idt_set_gate(20, (uint32_t) isr20, 0x08, 0x8E);
  idt_set_gate(21, (uint32_t) isr21, 0x08, 0x8E);
  idt_set_gate(22, (uint32_t) isr22, 0x08, 0x8E);
  idt_set_gate(23, (uint32_t) isr23, 0x08, 0x8E);
  idt_set_gate(24, (uint32_t) isr24, 0x08, 0x8E);
  idt_set_gate(25, (uint32_t) isr25, 0x08, 0x8E);
  idt_set_gate(26, (uint32_t) isr26, 0x08, 0x8E);
  idt_set_gate(27, (uint32_t) isr27, 0x08, 0x8E);
  idt_set_gate(28, (uint32_t) isr28, 0x08, 0x8E);
  idt_set_gate(29, (uint32_t) isr29, 0x08, 0x8E);
  idt_set_gate(30, (uint32_t) isr30, 0x08, 0x8E);
  idt_set_gate(31, (uint32_t) isr31, 0x08, 0x8E);

  remap_PIC(0x20, 0x28);

  idt_set_gate(32, (uint32_t) irq0, 0x08, 0x8E);
  idt_set_gate(33, (uint32_t) irq1, 0x08, 0x8E);
  idt_set_gate(34, (uint32_t) irq2, 0x08, 0x8E);
  idt_set_gate(35, (uint32_t) irq3, 0x08, 0x8E);
  idt_set_gate(36, (uint32_t) irq4, 0x08, 0x8E);
  idt_set_gate(37, (uint32_t) irq5, 0x08, 0x8E);
  idt_set_gate(38, (uint32_t) irq6, 0x08, 0x8E);
  idt_set_gate(39, (uint32_t) irq7, 0x08, 0x8E);
  idt_set_gate(40, (uint32_t) irq8, 0x08, 0x8E);
  idt_set_gate(41, (uint32_t) irq9, 0x08, 0x8E);
  idt_set_gate(42, (uint32_t) irq10, 0x08, 0x8E);
  idt_set_gate(43, (uint32_t) irq11, 0x08, 0x8E);
  idt_set_gate(44, (uint32_t) irq12, 0x08, 0x8E);
  idt_set_gate(45, (uint32_t) irq13, 0x08, 0x8E);
  idt_set_gate(46, (uint32_t) irq14, 0x08, 0x8E);
  idt_set_gate(47, (uint32_t) irq15, 0x08, 0x8E);

#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("Flushing initialized IDT at 0x%x.\n", &idt_ptr);
#endif
  idt_flush((uint32_t) &idt_ptr);
#ifdef _KERNEL_DEBUG
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  printf("[INFO] ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  printf("IDT flushed.\n");
#endif
}

static void gdt_set_gate(size_t num,
                         uint32_t base,
                         uint32_t limit,
                         uint8_t access,
                         uint8_t granularity) {
  gdt_entries[num].base_low_16 = (base & 0xFFFF);
  gdt_entries[num].base_mid_8  = (base >> 16) & 0xFF;
  gdt_entries[num].base_high_8 = (base >> 24) & 0xFF;

  gdt_entries[num].limit_low_16 = (limit & 0xFFFF);
  gdt_entries[num].granularity  = (limit >> 16) & 0x0F;

  gdt_entries[num].granularity |= granularity & 0xF0;
  gdt_entries[num].access       = access;
}

static void idt_set_gate(uint8_t num,
                         uint32_t base,
                         uint16_t selector,
                         uint8_t flags) {
  idt_entries[num].base_low_16  = (base & 0xFFFF);
  idt_entries[num].base_high_16 = (base >> 16) & 0xFFFF;

  idt_entries[num].selector = selector;

  idt_entries[num].zero = 0;

  idt_entries[num].flags = flags;
}
