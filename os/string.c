#include "string.h"

void* memcpy(void* destination, const void* source, unsigned long length) {
	unsigned char* destination_bytes = (unsigned char*)destination;
	const unsigned char* source_bytes = (const unsigned char*)source;

	for (unsigned long i = 0; i < length; i++) {
		destination_bytes[i] = source_bytes[i];
	}

	return destination;
}

unsigned long strlen(char* string) {
	unsigned long length = 0;

	while (string[length] != '\0') {
		length++;
	}

	return length;
}

unsigned char strcmp(char* string_1, char* string_2) {
	unsigned long length = strlen(string_1);

	if (length != strlen(string_2)) {
		return 1;
	}

	for (unsigned long i = 0; i < length; i++) {
		if (string_1[i] != string_2[i]) {
			return 1;
		}
	}

	return 0;
}

/*
char* string_copy(char* string, char* string_copy, unsigned long start, unsigned long end) {
	for (unsigned long i = 0; i < (end - start); i++) {
		string_copy[i] = string[start + i];
	}

	return string_copy;
}

char* string_append_character(char* string, char character) {
	unsigned long length = string_length(string);

	string[length] = character;
	string[length + 1] = '\0';

	return string;
}

char* string_append(char* string, char* string_append) {
	unsigned long length = string_length(string);
	unsigned long length_append = string_length(string_append);

	for (unsigned long i = 0; i < length_append; i++) {
		string[length + i] = string_append[i];
	}

	string[length + length_append] = '\0';

	return string;
}

char* string_delete(char* string, unsigned long start, unsigned long end) {
	unsigned long tail_length = string_length(string) - end;

	for (unsigned long i = 0; i <= tail_length; i++) {
		string[start + i] = string[end + i];
	}

	return string;
}
*/
