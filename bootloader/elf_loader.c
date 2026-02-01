#include "ata.h"


// work in progress elf driver stuff
#define ELF_PT_LOAD 1

#define ELF_EI_NIDENT 16

typedef struct {
	unsigned char e_ident[ELF_EI_NIDENT];
	unsigned short e_type;
	unsigned short e_machine;
	unsigned int e_version;
	unsigned long e_entry;
	unsigned long e_phoff;
	unsigned long e_shoff;
	unsigned int e_flags;
	unsigned short e_ehsize;
	unsigned short e_phentsize;
	unsigned short e_phnum;
	unsigned short e_shentsize;
	unsigned short e_shnum;
	unsigned short e_shstrndx;
} Elf64_Ehdr;

typedef struct {
	unsigned int p_type;
	unsigned int p_flags;
	unsigned long p_offset;
	unsigned long p_vaddr;
	unsigned long p_paddr;
	unsigned long p_filesz;
	unsigned long p_memsz;
	unsigned long p_align;
} Elf64_Phdr;
// end of elf driver stuff


__attribute__((section(".start")))
void main(void) {
	unsigned int lba = 0x00000000;
	unsigned char count = 1;
	unsigned short* buffer = (unsigned short*)0x35000;

	ata_prepare_read(lba, count);
	ata_read(buffer, count);

	__asm__ volatile ("hlt");
}
