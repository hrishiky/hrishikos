#include "idt.h"
#include "shell.h"

void main() {
	idt_init();

	vga_text_clear_screen();

	shell_main();

	__asm__("hlt");
}
