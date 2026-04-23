#include "fs_superblock.h"

#include "fs_system.h"

fs_superblock_t superblock;

void fs_superblock_init(void) {
	superblock.magic_number = FS_SYSTEM_MAGIC_NUMBER;
	superblock.start_sector = FS_SYSTEM_START_SECTOR;
	superblock.block_sector_count = FS_SYSTEM_BLOCK_SECTOR_COUNT;
	superblock.block_count = FS_SYSTEM_SECTOR_COUNT / FS_SYSTEM_BLOCK_SECTOR_COUNT;

	// compute based on inodes per block

	uint64_t inode_table_block_count = fs_size_to_blocks(FS_SYSTEM_INODE_COUNT * sizeof(fs_inode_t));

	superblock.inode_bitmap_start = fs_size_to_blocks(sizeof(fs_superblock_t));
	superblock.inode_bitmap_block_count = fs_size_to_blocks((inode_table_block_count + 7) / 8);
	superblock.inode_table_start = superblock.inode_bitmap_start + inode_bitmap_block_count + 1;
	superblock.inode_table_block_count = inode_table_block_count;
	superblock.inode_count = FS_SYSTEM_INODE_COUNT;

	superblock.data_bitmap_start = superblock.inode_table_start + superblock.inode_table_block_count + 1;
	superblock.data_bitmap_block_count = (superblock.block_count - superblock.inode_bitmap_block_count - superblock.inode_table_block_count) / 9;
	superblock.data_blocks_start = superblock.bitmap_start + superblock.bitmap_block_count + 1;
	superblock.data_blocks_block_count = superblock.data_bitmap_block_count * 8;

	superblock.bytes_per_sector = FS_BYTES_PER_SECTOR;
	superblock.bytes_per_block = superblock.block_sector_count * superblock.bytes_per_sector;

	ata_write(FS_SYSTEM_START_SECTOR, (void*) superblock, sizeof(fs_superblock_t));
}

bool fs_superblock_load(void) {
	ata_read(FS_SYSTEM_START_SECTOR, (void*) &superblock, fs_size_to_sectors(sizeof(fs_superblock_t)));

	if (superblock.magic_number != FS_SYSTEM_MAGIC_NUMBER) {
		return false;
	}

	return true;
}
