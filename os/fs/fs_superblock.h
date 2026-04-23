#ifndef FS_SUPERBLOCK_H
#define FS_SUPERBLOCK_H

#include "stdint.h"
#include "stdbool.h"

typedef struct {
	uint32_t magic_number;
	uint64_t start_sector;
	uint64_t block_sector_count;
	uint64_t block_count;
	uint64_t inodes_per_block;
	uint64_t inode_bitmap_start;
	uint64_t inode_bitmap_block_count;
	uint64_t inode_table_start;
	uint64_t inode_table_block_count;
	uint64_t inode_count;
	uint64_t data_bitmap_start;
	uint64_t data_bitmap_block_count;
	uint64_t data_blocks_start;
	uint64_t data_blocks_block_count;

	uint64_t bytes_per_sector;
	uint64_t bytes_per_block;
} __attribute__((packed)) fs_superblock_t;


void fs_superblock_init(void);
bool fs_superblock_load(void);

#endif
