#include "idt.h"
#include "vga_text.h"

void main() {
	idt_init();

	char* hello_world = "Hello World! FROM THE KERNEL!!!";
	vga_text_print(hello_world);

	__asm__("hlt");
}
