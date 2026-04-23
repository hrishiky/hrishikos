#ifndef FS_HELPER_H
#define FS_HELPER_H

#include "stdint.h"

uint64_t fs_size_to_blocks(uint64_t size);
uint64_t fs_block_to_sector(uint64_t start_sector, uint64_t block_sector_count, uint64_t block);
uint64_t fs_size_to_sectors(uint64_t size);

#endif
