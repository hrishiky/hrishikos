#include "fs.h"

#include "fs_object.h"
#include "fs_bitmap.h"

extern uint64_t fs_max_data_blocks;
extern uint64_t fs_used_data_blocks;

fs_superblock_t superblock;

uint64_t fs_size_to_blocks(uint64_t size) {
	return (size + FS_SYSTEM_BLOCK_SIZE - 1) / FS_SYSTEM_BLOCK_SIZE;
}

void fs_init(void) {
	fs_superblock_t init_superblock;

	init_superblock.magic_number = FS_SYSTEM_MAGIC_NUMBER;
	init_superblock.block_size = FS_SYSTEM_BLOCK_SIZE;
	init_superblock.total_block_count = FS_SYSTEM_SIZE / FS_SYSTEM_BLOCK_SIZE;
	init_superblock.inode_table_start = fs_size_to_blocks(sizeof(fs_superblock_t)) + 1;
	init_superblock.inode_table_block_count = fs_size_to_blocks(FS_SYSTEM_INODE_COUNT * sizeof(fs_inode_t));
	init_superblock.max_inode_count = FS_SYSTEM_INODE_COUNT;
	init_superblock.bitmap_start = init_superblock.inode_table_start + init_superblock.inode_table_block_count + 1;
	init_superblock.bitmap_block_count = ((init_superblock.total_block_count - init_superblock.inode_table_block_count) / 8) / FS_SYSTEM_BLOCK_SIZE;
	init_superblock.data_blocks_start = init_superblock.bitmap_start + init_superblock.bitmap_block_count + 1;

	ata_write(1, (void*) init_superblock, sizeof(fs_superblock_t));

	fs_bitmap_init();
	fs_superblock_load();
}

void fs_superblock_load(void) {
	ata_read(1, (void*) &superblock, fs_size_to_blocks(sizeof(fs_superblock_t)));

	fs_max_data_blocks =  superblock.total_block_count - superblock.inode_table_block_count - superblock.bitmap_block_count;
	fs_used_data_blocks = 0;
}
