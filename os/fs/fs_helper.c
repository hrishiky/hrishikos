#include "fs_helper.h"

#include "stdint.h"
#include "fs_superblock.h"
#include "fs_system.h"

extern fs_superblock_t superblock;

uint64_t fs_size_to_blocks(uint64_t size) {
	return (size + FS_SYSTEM_BLOCK_SIZE - 1) / FS_SYSTEM_BLOCK_SIZE;
}

uint64_t fs_block_to_sector(uint64_t block) {
	return superblock.start_sector + (block * superblock.block_sector_count);
}

uint64_t fs_size_to_sectors(uint64_t size) {
	return (size + FS_DISK_SECTOR_SIZE - 1) / FS_DISK_SECTOR_SIZE;
}
