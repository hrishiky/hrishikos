#include "vga_text.h"
#include "sys_io.h"

char* vga = (char*)0xB8000;
long vga_text_cursor_x = 0;
long vga_text_cursor_y = 0;
char* vga_text_hex_lookup = "0123456789ABCDEF";


void vga_text_blinking_cursor_enable(unsigned char start, unsigned char end) {
	outb(0x0A, 0x3D4);

	outb((inb(0x3D5) & 0xC0) | start, 0x3D5);

	outb(0x0B, 0x3D4);
	outb((inb(0x3D5) & 0xE0) | end, 0x3D5);
}

void vga_text_blinking_cursor_disable(void) {
	outb(0x0A, 0x3D4);
	outb(0x20, 0x3D5);
}

void vga_text_blinking_cursor_shift(void) {
	unsigned long position = vga_text_cursor_y * VGA_TEXT_COLUMNS + vga_text_cursor_x;

	outb(0x0F, 0x3D4);
	outb((unsigned int) (position & 0xFF), 0x3D5);

	outb(0x0E, 0x3D4);
	outb((unsigned int) ((position >> 8) & 0xFF), 0x3D5);
}

void vga_text_scroll(long count) {
	;
}

void vga_text_cursor_rectify(void) {
	if (vga_text_cursor_x > VGA_TEXT_COLUMNS_MAXIMUM) {
		unsigned long overflow = vga_text_cursor_x / VGA_TEXT_COLUMNS;
		vga_text_cursor_x -= VGA_TEXT_COLUMNS * overflow;
		vga_text_cursor_y += overflow;
	}

	if (vga_text_cursor_x < VGA_TEXT_COLUMNS_MINIMUM) {
		unsigned long overflow = (vga_text_cursor_x / VGA_TEXT_COLUMNS) + 1;
		vga_text_cursor_x -= VGA_TEXT_COLUMNS * overflow;
		vga_text_cursor_y += overflow;
	}

	if (vga_text_cursor_y > VGA_TEXT_ROWS_MAXIMUM) {
		unsigned long overflow = vga_text_cursor_y - VGA_TEXT_ROWS_MAXIMUM;
		vga_text_scroll(overflow);
		vga_text_cursor_y = VGA_TEXT_ROWS_MAXIMUM;
	}

	if (vga_text_cursor_y < VGA_TEXT_ROWS_MINIMUM) {
		vga_text_scroll(vga_text_cursor_y);
		vga_text_cursor_y = VGA_TEXT_ROWS_MINIMUM;
	}
}

void vga_text_cursor_shift(long x, long y) {
	vga_text_cursor_x = x;
	vga_text_cursor_y = y;

	vga_text_cursor_rectify();

	vga_text_blinking_cursor_shift();
}

void vga_text_clear_screen(void) {
	unsigned long long* buffer = (unsigned long long*)vga;

	for (unsigned short i = 0; i < VGA_TEXT_BUFFER_LONG_LONG_COUNT; i++) {
		buffer[i] = 0;
	}

	vga_text_cursor_shift(0, 0);
}

void vga_text_print_character(char character) {
	switch (character) {
		case '\n':
			vga_text_cursor_y++;
			vga_text_cursor_shift(0, vga_text_cursor_y);
			return;

		case '\b':
			vga_text_cursor_x--;
			vga_text_cursor_shift(vga_text_cursor_x, vga_text_cursor_y);

			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x)] = ' ';
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x) + 1] = VGA_TEXT_BW;

			return;

		default:
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x)] = character;
			vga[2 * ((vga_text_cursor_y * VGA_TEXT_COLUMNS) + vga_text_cursor_x) + 1] = VGA_TEXT_BW;

			vga_text_cursor_x++;
			vga_text_cursor_shift(vga_text_cursor_x, vga_text_cursor_y);

			return;
	}
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
	for (unsigned long i = 0; string[i] != '\0'; i++) {
		vga_text_print_character(string[i]);
	}
}

void vga_text_print_hex(unsigned char* data, unsigned int byte_count, unsigned char number_length) {
	for (unsigned int i = 0; i < byte_count; i++) {
		if (i % (number_length / 2) == 0) {
			vga_text_print(" 0x");
		}

		vga_text_print_character(vga_text_hex_lookup[data[i]]);
	}
}
