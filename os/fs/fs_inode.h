#ifndef FS_INODE_H
#define FS_INODE_H

#include "stdint.h"

#define FS_INODE_CACHE_SIZE 128

#define FS_INODE_NAME_MAX_LENGTH 16
#define FS_INODE_TABLE_BLOCK_COUNT 8
#define FS_INODE_INDIRECT_TABLE_BLOCK_COUNT 64
#define FS_INODE_TYPE_FILE 0
#define FS_INODE_TYPE_DIRECTORY 1

typedef struct {
	uint64_t blocks[FS_SYSTEM_];
} fs_indirect_table_t;

typedef struct {
	uint8_t type;
	size_t size;
	uint64_t block_count;
	uint64_t blocks[FS_INODE_TABLE_BLOCK_COUNT];
	uint64_t first_indirect_table;
	uint64_t second_indirect_table;
	uint64_t third_indirect_table;
} __attribute__((packed)) fs_inode_t;

typedef struct {
	fs_inode_t inode;
	uint64_t index;
	uint8_t reference;
	bool dirty;
	bool valid;
} __attribute__((packed)) fs_inode_cache_entry_t;

#endif
