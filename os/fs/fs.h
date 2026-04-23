#ifndef FS_H
#define FS_H

#include "stdint.h"

#define FS_SYSTEM_MAGIC_NUMBER (0xFF << 24) & ('F' << 16) & ('S' << 8) & (0x00)
#define FS_SYSTEM_BLOCK_SIZE 512
#define FS_SYSTEM_SIZE 10
#define FS_SYSTEM_INODE_COUNT 1024

typedef struct {
	uint32_t magic_number;
	uint64_t block_size;
	uint64_t total_block_count;
	uint64_t bitmap_start;
	uint64_t bitmap_block_count;
	uint64_t inode_table_start;
	uint64_t inode_table_block_count;
	uint64_t max_inode_count;
	uint64_t data_blocks_start;
} fs_superblock_t;

#endif
