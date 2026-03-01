#ifndef VGA_TEXT_H
#define VGA_TEXT_H

#define VGA_TEXT_ROWS 25
#define VGA_TEXT_ROWS_MINIMUM 0
#define VGA_TEXT_ROWS_MAXIMUM 24

#define VGA_TEXT_COLUMNS 80
#define VGA_TEXT_COLUMNS_MINIMUM 0
#define VGA_TEXT_COLUMNS_MAXIMUM 79

#define VGA_TEXT_BW 0x0F
#define VGA_TEXT_BUFFER_LONG_LONG_COUNT 500

void vga_text_blinking_cursor_enable(unsigned char start, unsigned char end);
void vga_text_blinking_cursor_disable(void);
void vga_text_blinking_cursor_shift(void);
void vga_text_scroll(long count);
void vga_text_cursor_rectify(void);
void vga_text_cursor_shift(long x, long y);
void vga_text_clear_screen(void);
void vga_text_print_character(char character);
void vga_text_print_position(char* string, unsigned char x, unsigned char y);
void vga_text_print(char* string);
void vga_text_print_hex(unsigned char* data, unsigned int byte_count, unsigned char number_length);

#endif
