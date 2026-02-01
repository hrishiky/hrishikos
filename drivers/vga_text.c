#include "vga_text.h"

char* vga = (char*)0xB8000;

int strlen(char* string) {
        int length = 0;

        while (string[length] != '\0') {
                length++;
        }

        return length;
}

void print(char* string, int x, int y) {
        for(int i = 0; i < strlen(string); i++) {
                if (x > 80) {
                        y++;
                        x -= 80;
                } else if (y > 25) {
                        y -= 25;
                }

                vga[2 * ((y * 80) + x)] = string[i];
                vga[2 * ((y * 80) + x) + 1] = 0x0F;

                x++;
        }
}
