#include <stdio.h>

#include <kernel/tty.h>

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
  terminal_initialize();

	terminal_writestring("Hello, kernel World!\n");
	terminal_writestring("Hello, newline works!\n");
  terminal_clear();
	terminal_writestring("Clear works!\n");
	terminal_writestring(
    "This is a test trying to write across multiple lines to test if lines "
    "are correctly wrapped.\n"
  );
  terminal_writestring(
    "This\nis\na\ntest\ntrying\nto\nwrite\nacross\nthe\nwhole\nscreen\nto\n"
    "test\nif\nup\nscrolling\nis\nworking\nor\nnot.\n"
  );
  terminal_writestring("\n\n");
  terminal_writestring("Hello, kernel World!");
}
