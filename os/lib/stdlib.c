#include "stdlib.h"
#include "heap.h"
#include "string.h"

char* itoa(long number, char* buffer, unsigned char base) {
	char* base_lookup = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (base < 2 ||
	    base > 36) {
		return buffer;
	}

	if (number == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return (char*) buffer;
	}

	unsigned char digit;
	unsigned short i = 0;
	unsigned char negative = 0;

	if (number < 0) {
		negative = 1;
		number *= -1;
	}

	while (number != 0) {
		digit = number % base;
		buffer[i] = base_lookup[digit];
		number /= base;
		i++;
	}

	if (negative) {
		buffer[i++] = '-';
	}

	buffer[i] = '\0';

	strrev((char*) buffer);

	return (char*) buffer;
}
