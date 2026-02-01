#include "ata.h"
#include "elf.h"

__attribute__((section(".start")))
void main(void) {
	unsigned int lba = 0x00000043;
	unsigned char count = 31;
	unsigned short* buffer = (unsigned short*)0x35000;

	ata_prepare_read(lba, count);
	ata_read(buffer, count);


	void* elf = (void*)0x35000;

	elf_check_header(elf);
	elf_load_program_headers(elf);
	elf_jump_entry_point(elf);
}
