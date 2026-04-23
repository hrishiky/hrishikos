#include "ata.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "asm_wrappers.h"

bool ata_irq_flag = false;

void ata_init(void) {
	;
}

void ata_check_error(void) {
	if ((inb(ATA_REGISTER_STATUS) & ATA_STATUS_ERR) != 0) {
		// vga_text_print(ATA_ERROR_MESSAGE);
		// __asm__ volatile ("hlt");
	}
}

void ata_wait_bsy(void) {
	while (inb(ATA_REGISTER_STATUS) & ATA_STATUS_BSY) {}
}

void ata_wait_drdy(void) {
	while (1) {
		unsigned char status = inb(ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			printf("ata error\n");
			return; // error
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
			printf("ata error\n");
			return; // error
		}

		if (!(status & ATA_STATUS_BSY) &&
		    (status & ATA_STATUS_DRQ)) {
			return;
		}
	}
}

void ata_read(uint64_t lba, void* buffer, uint8_t sector_count) {
	lba &= ATA_LBA_MASK;
	uint16_t* buf = (uint16_t*) buffer;

	ata_wait_bsy();

	outb(sector_count, ATA_REGISTER_SECTORCOUNT);
	outb(lba & 0xFF, ATA_REGISTER_LBA0);
	outb((lba >> 8) & 0xFF, ATA_REGISTER_LBA1);
	outb((lba >> 16) & 0xFF, ATA_REGISTER_LBA2);
	outb(ATA_DRIVE_MASTERLBA | ((lba >> 24) & 0x0F), ATA_REGISTER_DRIVEHEAD);

	ata_wait_drdy();

	outb(ATA_COMMAND_READ, ATA_REGISTER_COMMAND);

	for (uint8_t i = 0; i < sector_count; i++) {
		ata_wait_drq();

		rep_insw(buf, ATA_SECTOR_WORD_COUNT, ATA_REGISTER_DATA);
		buf += ATA_SECTOR_WORD_COUNT;

		ata_check_error();

		ata_wait_bsy();
	}
}

void ata_write(uint64_t lba, void* buffer, uint64_t data_count) {
	lba &= ATA_LBA_MASK;
	uint64_t data_word_count = data_count / 2;
	uint8_t sector_count = (data_word_count + ATA_SECTOR_WORD_COUNT - 1) / ATA_SECTOR_WORD_COUNT;
	uint16_t* buf = (uint16_t*) buffer;

	ata_wait_bsy();

	outb(sector_count, ATA_REGISTER_SECTORCOUNT);
	outb(lba & 0xFF, ATA_REGISTER_LBA0);
	outb((lba >> 8) & 0xFF, ATA_REGISTER_LBA1);
	outb((lba >> 16) & 0xFF, ATA_REGISTER_LBA2);
	outb(ATA_DRIVE_MASTERLBA | ((lba >> 24) & 0x0F), ATA_REGISTER_DRIVEHEAD);

	ata_wait_drdy();

	outb(ATA_COMMAND_WRITE, ATA_REGISTER_COMMAND);

	for (uint8_t i = 0; i < sector_count; i++) {
		while (!ata_irq_flag) {}

		printf("ata recieived\n");

		uint8_t status = inb(ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			; //err
		}

		if (!(status & ATA_STATUS_DRQ)) {
			; //err
		}

		if (data_word_count >= 256) {
			for (uint16_t j = 0; j < 256; j++) {
				outw(buf[j], ATA_REGISTER_DATA);
			}
		} else {
			for (uint8_t j = 0; j < data_word_count; j++) {
				outw(buf[j], ATA_REGISTER_DATA);
			}

			for (uint8_t j = 0; j < ATA_SECTOR_WORD_COUNT - data_word_count; j++) {
				outw(0, ATA_REGISTER_DATA);
			}
		}

		data_word_count -= 256;
		ata_irq_flag = false;
	}

	// ata command 0xE7
}
