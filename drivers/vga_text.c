#include "vga_text.h"

char* vga = (char*)0xB8000;
unsigned short CursorX = 0;
unsigned short CursorY = 0;
char* hex_lookup = "0123456789ABCDEF";

void vga_text_cursor_rectify(void) {
	if (CursorX >= VGA_TEXT_COLUMNS) {
		CursorX -= VGA_TEXT_COLUMNS;
		CursorY++;
	} else if (CursorY >= VGA_TEXT_ROWS) {
 		CursorY -= VGA_TEXT_ROWS;
	}
}

void vga_text_clear_screen(void) {
	unsigned long long* buffer = (unsigned long long*)vga;

	for (unsigned short i = 0; i < VGA_TEXT_BUFFER_LONG_LONG_COUNT; i++) {
		buffer[i] = 0;
	}

	CursorX = 0;
	CursorY = 0;
}

void vga_text_print_character(char character) {
	vga[2 * ((CursorY * VGA_TEXT_COLUMNS) + CursorX)] = character;
	vga[2 * ((CursorY * VGA_TEXT_COLUMNS) + CursorX) + 1] = VGA_TEXT_BW;

	CursorX++;

	vga_text_cursor_rectify();
}


void vga_text_print_position(char* string, unsigned char x, unsigned char y) {
	for (unsigned short i = 0; string[i] != '\0'; i++) {
		if (string[i] == '\n') {
			x = 0;
			y++;
			continue;
		}

		if (x >= VGA_TEXT_COLUMNS) {
			x -= VGA_TEXT_COLUMNS;
			y++;
		} else if (y >= VGA_TEXT_ROWS) {
 			y -= VGA_TEXT_ROWS;
		}

		vga[2 * ((y * VGA_TEXT_COLUMNS) + x)] = string[i];
		vga[2 * ((y * VGA_TEXT_COLUMNS) + x) + 1] = VGA_TEXT_BW;

		x++;
	}
}

void vga_text_print(char* string) {
	for (unsigned short i = 0; string[i] != '\0'; i++) {
		if (string[i] == '\n') {
			CursorX = 0;
			CursorY++;
			continue;
		}

		vga_text_cursor_rectify();

		vga[2 * ((CursorY * VGA_TEXT_COLUMNS) + CursorX)] = string[i];
		vga[2 * ((CursorY * VGA_TEXT_COLUMNS) + CursorX) + 1] = VGA_TEXT_BW;

		CursorX++;
	}
}

void vga_text_print_hex(unsigned char* data, unsigned int byte_count, unsigned char number_length) {
	for (unsigned int i = 0; i < byte_count; i++) {
		if (i % (number_length / 2) == 0) {
			vga_text_print(" 0x");
		}

		vga_text_print_character(hex_lookup[data[i] >> 4]);
		vga_text_print_character(hex_lookup[data[i] & 0x0F]);
	}
}
