#include <stdio.h>

#include <kernel/tty.h>

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
  terminal_initialize();

	printf("Hello, kernel World!\n");
	printf("Hello, newline works!\n");
  terminal_clear();
	printf("Clear works!\n");
	printf(
    "This is a test trying to write across multiple lines to test if lines "
    "are correctly wrapped.\n"
  );
  printf(
    "This\nis\na\ntest\ntrying\nto\nwrite\nacross\nthe\nwhole\nscreen\nto\n"
    "test\nif\nup\nscrolling\nis\nworking\nor\nnot.\n"
  );
  printf("\n\n");
  printf("Hello, kernel World!");
}
