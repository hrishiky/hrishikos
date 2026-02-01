#ifndef ATA_H
#define ATA_H

#include "vga_text.h"

void outb(unsigned char data, unsigned short port);
unsigned char inb(unsigned short port);
void rep_insw(unsigned short* buffer, unsigned short count, unsigned short port);

void ata_check_error(void);
void ata_wait_bsy(void);
void ata_wait_drdy(void);
void ata_wait_drq(void);
void ata_prepare_read(unsigned int lba, unsigned char count);
void ata_read(unsigned short* buffer, unsigned char count);

#endif
