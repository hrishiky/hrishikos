#include "idt.h"
#include "vga_text.h"
#include "keyboard.h"

void main() {
	vga_text_clear_screen();

	idt_init();

	vga_text_print("hello world\n\n");

	vga_text_print("type here: ");

	while (1) {
		/*
		unsigned long long i = 0;

		if (i % 256  == 0) {
			vga_text_print_position("|", 0, 0);
		} else if (i % 256 == 64) {
			vga_text_print_position("/", 0, 0);
		} else if (i % 256 == 128) {
			vga_text_print_position("-", 0, 0);
		} else if (i % 256 == 192) {
			vga_text_print_position("\\", 0, 0);
		}

		i++;
		*/

		char character = keyboard_buffer_read();

		if (character != -1) {
			vga_text_print_character(character);
		}
	}

	__asm__("hlt");
}
