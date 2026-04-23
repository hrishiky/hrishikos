#ifndef BITMAP_H
#define BITMAP_H

#include "stdint.h"
#include "stdbool.h"

#define BITMAP_DISK_SECTOR_UINT64_COUNT 64
#define BITMAP_DISK_SECTOR_BIT_COUNT 512 * 8

typedef struct {
	uint64_t* bitmap;
	uint64_t size;
} bitmap_memory_t;

typedef struct {
	uint64_t* block;
	bool dirty;
	uint64_t block_index;
	uint64_t used_bits;
	uint64_t total_bits;
	uint64_t start_sector;
	uint64_t block_sector_count;
	uint64_t block_count;
} bitmap_disk_t;



#endif
