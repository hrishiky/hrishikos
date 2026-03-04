#include "mem.h"

void* memcpy(void* destination, const void* source, unsigned long count) {
	unsigned char* destination_bytes = (unsigned char*) destination;
	const unsigned char* source_bytes = (const unsigned char*) source;

	for (unsigned long i = 0; i < count; i++) {
		destination_bytes[i] = source_bytes[i];
	}

	return destination;
}

void* memset(void *pointer, int value, unsigned long count) {
	unsigned char* pointer_byte = (unsigned char*) pointer;

	for (unsigned long i = 0; i < count; i++) {
		*pointer_byte = (unsigned char) value;
		pointer_byte++;
	}

	return pointer;
}
