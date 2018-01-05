.global gdt_flush
.type gdt_flush, @function
gdt_flush:
  movl 4(%esp), %eax
  lgdt (%eax)
  ljmpl $0x08, $_reload
_reload:
  mov  $0x10, %ax
  mov  %ax, %ds
  mov  %ax, %es
  mov  %ax, %fs
  mov  %ax, %gs
  mov  %ax, %ss
  ret

.global idt_flush
.type idt_flush, @function
idt_flush:
  movl 4(%esp), %eax
  lidt (%eax)
  ret
