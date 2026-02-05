#include "vga_text.h"

#define VGA_TEXT_ROWS 25
#define VGA_TEXT_COLUMNS 80

#define VGA_TEXT_BW 0x0F

char* vga = (char*)0xB8000;

unsigned short CursorX = 0;
unsigned short CursorY = 0;

unsigned short strlen(char* string) {
	unsigned short length = 0;

	while(1) {
		if (string[length] == '\0') {
			return length;
		}

		length++;
	}
}

void shift_cursor(unsigned short offset) {
	CursorX += offset;

	while (CursorX >= VGA_TEXT_COLUMNS) {
		CursorX -= VGA_TEXT_COLUMNS;
		CursorY++;

		if (CursorY >= VGA_TEXT_ROWS) {
			CursorY -= VGA_TEXT_ROWS;
		}
	}

}

void vga_text_print_position(char* string, unsigned char x, unsigned char y) {
	for (unsigned short i = 0; i < strlen(string); i++) {
		if (x > 80) {
			y++;
			x -= VGA_TEXT_COLUMNS;
		} else if (y > 25) {
 			y -= VGA_TEXT_COLUMNS;
		}

		vga[2 * ((y * VGA_TEXT_COLUMNS) + x)] = string[i];
		vga[2 * ((y * VGA_TEXT_COLUMNS) + x) + 1] = VGA_TEXT_BW;

		x++;
	}
}

void vga_text_print(char* string) {
	vga_text_print_position(string, CursorX, CursorY);
	shift_cursor(strlen(string));
}
