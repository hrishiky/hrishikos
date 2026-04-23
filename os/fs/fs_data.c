#include "fs_bitmap.h"

#include "fs_superblock.h"
#include "fs_helper.c"
#include "ata.h"
#include "bitmap.h"

extern fs_superblock_t superblock;

bitmap_disk_t data_bitmap;

void fs_data_init(void) {
	bitmap_disk_init(
		data_bitmap,
		fs_block_to_sector(superblock.data_bitmap_start, superblock.start_sector, superblock.block_sector_count),
		superblock.block_sector_count,
		superblock.data_bitmap_block_count,
		superblock.data_blocks_block_count
	);
}

uint64_t fs_data_alloc_block(void) {
	uint64_t block = bitmap_disk_find_free(&data_bitmap);

	if (!block) {
		return 0;
	}

	bitmap_disk_set(&data_bitmap, block);

	return block;
}

void fs_data_free_block(uint64_t index) {
	bitmap_disk_unset(&data_bitmap, index);
}

void fs_data_read_block(uint64_t block, void* buffer) {
	ata_read(fs_block_to_sector(superblock.data_start + block), buffer, superblock.block_sector_count);
}

void fs_data_write_block(uint64_t block, void* buffer, uint16_t count) {
	if (count > FS_BYTES_PER_SECTOR) {
		count = FS_BYTES_PER_SECTOR;
	}

	ata_write(fs_block_to_sector(superblock.data_start + block), buffer, count);
}
