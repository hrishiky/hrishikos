#include "ata.h"
#include "vga_text.h"

#define ATA_REGISTER_DATA 0x1F0
#define ATA_REGISTER_SECTORCOUNT 0x1F2
#define ATA_REGISTER_LBA0 0x1F3
#define ATA_REGISTER_LBA1 0x1F4
#define ATA_REGISTER_LBA2 0x1F5
#define ATA_REGISTER_DRIVEHEAD 0x1F6
#define ATA_REGISTER_COMMAND 0x1F7
#define ATA_REGISTER_STATUS 0x1F7

#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_ERR 0x01

#define ATA_COMMAND_READ 0x20

#define ATA_DRIVE_MASTERLBA 0xE0

#define ATA_LBA_MASK 0x0FFFFFFF

#define SECTOR_WORD_COUNT 0x100

#define ATA_ERROR_MESSAGE "ata disk read failed"
#define ATA_SUCCESS_MESSAGE "ata disk read done"

void outb(unsigned char data, unsigned short port) {
	__asm__ volatile (
		"outb %0, %1"
		:
		: "a"(data), "d"(port)
	);
}


unsigned char inb(unsigned short port) {
	unsigned char data;

	__asm__ volatile (
		"inb %1, %0"
		: "=a"(data)
		: "d"(port)
	);

	return data;

}

void rep_insw(unsigned short* buffer, unsigned short count, unsigned short port) {
	__asm__ volatile (
		"rep insw"
		: "+D"(buffer), "+c"(count)
		: "d"(port)
		: "memory"
	);
}


void ata_check_error(void) {
	if ((inb(ATA_REGISTER_STATUS) & ATA_STATUS_ERR) != 0) {
		print(ATA_ERROR_MESSAGE, 0, 0);
		__asm__ volatile ("hlt");
	}
}

void ata_wait_bsy(void) {
	while (inb(ATA_REGISTER_STATUS) & ATA_STATUS_BSY) {
		;
	}
}

void ata_wait_drdy(void) {
	while (1) {
		unsigned char status = inb(ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			__asm__ volatile ("hlt");
		}

		if ((status & ATA_STATUS_DRDY) != 0) {
			return;
		}
	}
}

void ata_wait_drq(void) {
	while (1) {
		unsigned char status = inb(ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			__asm__ volatile ("hlt");
		}

		if (!(status & ATA_STATUS_BSY) &&
		    (status & ATA_STATUS_DRQ)) {
			return;
		}
	}
}

void ata_prepare_read(unsigned int lba, unsigned char count) {
	lba &= ATA_LBA_MASK;

	ata_wait_bsy();

	outb(count, ATA_REGISTER_SECTORCOUNT);
	outb(lba & 0xFF, ATA_REGISTER_LBA0);
	outb((lba >> 8) & 0xFF, ATA_REGISTER_LBA1);
	outb((lba >> 16) & 0xFF, ATA_REGISTER_LBA2);
	outb(ATA_DRIVE_MASTERLBA | ((lba >> 24) & 0x0F), ATA_REGISTER_DRIVEHEAD);

	ata_wait_drdy();

	outb(ATA_COMMAND_READ, ATA_REGISTER_COMMAND);
}

void ata_read(unsigned short* buffer, unsigned char count) {
	for (unsigned char i = 0; i < count; i++) {
		ata_wait_drq();

		rep_insw(buffer, SECTOR_WORD_COUNT, ATA_REGISTER_DATA);
		buffer += SECTOR_WORD_COUNT;

		ata_check_error();

		ata_wait_bsy();
	}

	print(ATA_SUCCESS_MESSAGE, 0, 0);
}
