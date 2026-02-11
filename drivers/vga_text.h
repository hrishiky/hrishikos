#ifndef VGA_TEXT_H
#define VGA_TEXT_H

#define VGA_TEXT_ROWS 25
#define VGA_TEXT_COLUMNS 80
#define VGA_TEXT_BW 0x0F
#define VGA_TEXT_BUFFER_LONG_LONG_COUNT 500

void vga_text_cursor_rectify(void);
void vga_text_clear_screen(void);
void vga_text_print_character(char character);
void vga_text_print_position(char* string, unsigned char x, unsigned char y);
void vga_text_print(char* string);
void vga_text_print_hex(unsigned char* data, unsigned int byte_count, unsigned char number_length);

#endif
