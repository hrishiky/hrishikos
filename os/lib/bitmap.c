#include "bitmap.h"

uint64_t bitmap_disk_bit_to_block(bitmap_disk_t* bitmap, uint64_t bit) {
	return bit / (bitmap->block_sector_count * BITMAP_DISK_SECTOR_BIT_COUNT);
}

bitmap_disk_t* bitmap_disk_init(bitmap_disk_t* bitmap, uint64_t start_sector, uint64_t block_sector_count, uint64_t block_count, uint64_t size) {
	bitmap.start_sector = start_sector;
	bitmap.block_sector_count = block_sector_count;
	bitmap.block_count = block_count;
	bitmap.total_bits = size;

	uint64_t sector_model[BITMAP_DISK_SECTOR_UINT64_COUNT];

	for (uint8_t i = 0; i < BITMAP_DISK_SECTOR_UINT64_COUNT; i++) {
		sector_model[i] = 0;
	}

	for (uint64_t sector = 0; sector < (block_count * block_sector_count) - 1; sector++) {
		ata_write(start_sector + sector; (void*) sector_model; 1);
	}

	uint64_t last_sector_bits = size - (block_sector_count * (block_count - 1) * BITMAP_DISK_SECTOR_BIT_COUNT)

	for (uint64_t bit = 0; bit < last_sector_bits; bit++) {
		sector_model[(bit / 8) % (block_sector_count * 512)] &= ~(1ULL << (bit % 64));
	}

	for (uint64_t bit = last_sector_bits; bit < BITMAP_DISK_SECTOR_BIT_COUNT; bit++) {
		sector_model[(bit / 8) % (block_sector_count * 512)] |= (1ULL << (bit % 64));
	}

	ata_write(start_sector + (block_count * block_sector_count), (void*) sector_model, 1);
}

void bitmap_disk_load_block(bitmap_disk_t* bitmap, uint64_t block) {
	if (block > bitmap->block_count) {
		return;
	}

	if (bitmap->dirty) {
		ata_write(bitmap->start_sector + (bitmap->index * bitmap->block_sector_count), (void*) &bitmap->block, bitmap->block_sector_count);
	}

	ata_read(bitmap->start_sector + (block * bitmap->block_sector_count), (void*) &bitmap->block, bitmap->block_sector_count);

	bitmap->index = block;
	bitmap->dirty = false;
}

void bitmap_disk_checked_load_block(bitmap_disk_t* bitmap, uint64_t bit) {
	uint64_t block = bitmap_disk_bit_to_block(bitmap, bit);

	if (bitmap->index != block) {
		bitmap_disk_load_block(block);
	}
}

void bitmap_disk_set(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit > bitmap->total_bits) {
		return;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	bitmap->block[(bit / 8) % fs_block_size] |= (1ULL << (bit % 64));

	bitmap->used_bits++;
	bitmap->dirty = true;
}

void bitmap_disk_unset(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit > bitmap->total_bits) {
		return;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	bitmap->block[(bit / 8) % fs_block_size] &= ~(1ULL << (bit % 64));

	bitmap->used_bits--;
	bitmap->dirty = true;
}

bool bitmap_disk_test(bitmap_disk_t* bitmap, uint64_t bit) {
	if (bit > bitmap->total_bits) {
		return false;
	}

	bitmap_disk_checked_load_block(bitmap, bit);

	if (bitmap->block[(bit / 8) % fs_block_size] & (1ULL << (bit % 64))) {
		return true;
	}

	return false;
}

uint64_t bitmap_disk_find_free_bit(bitmap_disk_t* bitmap) {
	if (bitmap->used_bits >= bitmap->total_bits) {
		return 0;
	}

	uint64_t current_block = bitmap->index;

	for (uint64_t i = 0; i < bitmap->block_count; i++) {
		for (uint64_t j = 0; j < (bitmap->block_sector_count / 512) / 8; j++) {
			if (bitmap->block[j] == 0xFFFFFFFFFFFFFFFF) {
				continue;
			}

			for (uint8_t k = 0; k < BITMAP_UINT64_BIT_COUNT; k++) {
				uint64_t bit = (j * BITMAP_UINT64_BIT_COUNT) + k;

				if (bitmap_disk_test(bitmap, bit)) {
					return bit;
				}
			}
		}

		if (current_block++ > bitmap->block_count) {
			current_block -= bitmap->block_count;
		}

		bitmap_disk_load_block(current_block);
	}

	return 0;
}
