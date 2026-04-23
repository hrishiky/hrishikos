#include "fs_inode.h"

#include "fs_helper.h"
#include "fs_data.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ata.h"
#include "bitmap.h"
#include "math.h"

/* replace all consts with superblock values remove hardcoded stuff */

extern fs_superblock_t superblock;

bitmap_disk_t inode_bitmap;
fs_inode_cache_entry_t inode_cache[FS_INODE_CACHE_SIZE];
uint64_t inode_cache_clock_hand;

void fs_inode_init(void) {
	bitmap_disk_init(
		inode_bitmap,
		fs_block_to_sector(superblock.inode_bitmap_start, superblock.start_sector, superblock.block_sector_count),
		superblock.block_sector_count,
		superblock.inode_bitmap_block_count,
		superblock.inode_count
	);

	for (uint64_t i = 0; i < FS_INODE_CACHE_SIZE; i++) {
		inode_cache[i].valid = false;
	}

	inode_cache_clock_hand = 0;
}

void fs_inode_cache_entry_flush(uint64_t index) {
	if (!inode_cache[index].dirty) {
		return;
	}

	// ata_write();
}

uint64_t fs_inode_cache_entry_evict(void) {
	if (!inode_cache[inode_cache_clock_hand].valid) {
		continue;
	}

	if (!inode_cache[inode_cache_clock_hand].reference) {
		inode_cache[inode_cache_clock_hand].valid = false;
		fs_inode_cache_entry_flush(inode_cache_clock_hand);
		inode_cache_clock_hand++;

		return inode_cache_block_hand - 1;
	}

	inode_cache[inode_cache_clock_hand].reference = false;
	inode_cache_clock_hand++;

	if (inode_cache_clock_hand >= FS_INODE_CACHE_SIZE) {
		inode_cache_clock_hand = 0;
	}

	return 0;
}

uint64_t fs_inode_cache_evict(void) {
	for (uint64_t i = 0; i < FS_INODE_CACHE_SIZE; i++) {
		uint64_t index = fs_inode_cache_entry_evict();

		if (index) {
			return index;
		}
	}

	// manually find first valid

	return 0;
}

uint64_t fs_inode_cache_add(fs_inode_t inode, uint64_t index) {
	fs_inode_cache_entry_t entry;

	entry->inode = inode;
	entry->index = index;
	entry->reference = 1;
	entry->dirty = false;
	entry->valid = true;

	uint64_t cache_index = fs_inode_cache_evict();
	inode_cache[cache_index] = entry;

	return cache_index;
}

fs_inode_cache_entry_t* fs_inode_cache_search(uint64_t index) {
	for (uint64_t i = 0; i < FS_INODE_CACHE_SIZE; i++) {
		if (!inode_cache[i].valid) {
			continue;
		}

		if (inode_cache[i].index == index) {
			return &inode_cache[i];
		}
	}

	return NULL;
}

uint64_t fs_inode_create(uint8_t type, char* name) { //add file path
	uint64_t index = bitmap_disk_find_free_bit(&inode_bitmap);

	if (!index) {
		return 0;
	}

	fs_inode_t inode;
	inode.type = type;
	inode.size = 0;
	inode.block_count = 0;

	fs_inode_cache_add(inode, index);

	bitmap_disk_set(&inode_bitmap, index);

	return index;
}

fs_inode_t fs_inode_table_get(uint64_t inode_table_block, uint64_t table_index) {
	if (table_index >= superblock.inodes_per_block) {
		return 0;
	}

	uint8_t buffer[512];

	ata_read(fs_block_to_sector(superblock.inode_table_start + inode_table_block), (void*) buffer, 1); // 1 sector hardcoded

	return ((fs_inode_t*) buffer)[table_index];
}

fs_inode_cache_entry_t* fs_inode_get(uint64_t index) {
	fs_inode_cache_entry_t* cache_entry = fs_inode_cache_search(index);

	if (cache_entry) {
		return cache_entry;
	}

	uint64_t inode_table_block = index / superblock.inodes_per_block;
	uint64_t inode_table_block_index = index % superblock.inodes_per_block;

	fs_inode_t inode = fs_inode_table_get(inode_table_block, inode_table_block_index);

	uint64_t cache_index = fs_inode_cache_add(inode, index);

	return &inode_cache[cache_index];
}

void fs_inode_set_size(fs_inode_cache_entry_t* inode, uint64_t size) {
	inode->inode->size = size;
	inode->reference = 1;
	inode->dirty = true;
}

void fs_inode_add_data_blocks(fs_inode_cache_entry_t* icache, uint64_t count) {
	uint64_t table = FS_INODE_TABLE_BLOCK_COUNT;
	uint64_t indirect = FS_INODE_INDIRECT_TABLE_BLOCK_COUNT;

	if (count + icache->inode->block_count > table + indirect + pow(indirect, 2) + pow(indirect, 3)) {
		return;
	}

	for (uint64_t i = 0; i < count; i++) {
		uint64_t block_count = cache->inode->block_count;
		uint64_t new_data_block = fs_data_alloc_block();

		if (block_count <= table) {
			icache->inode->blocks[block_count] = new_data_block;
		} else if (block_count <= table + indirect) {
			uint64_t index = block_count - table;
			fs_indirect_table_t* buffer;

			fs_data_block_read(icache->inode->first_indirect_table, (void*) buffer);
			buffer->blocks[index] = new_data_block;
			fs_data_block_write(icache->inode->first_indirect_table, (void*) buffer, FS_SYSTEM_BYTES_PER_BLOCK);
		} else if (block_count <= table + indirect + pow(indirect, 2)) {
			uint64_t index_1 = (block_count - table) / indirect;
			uint64_t index_2 = (block_count - table) % indirect;
			fs_indirect_table_t* buffer;

			fs_data_block_read(ichache->inode->first_indirect_table, (void*) buffer);

			uint64_t block = buffer->blocks[index_1];

			fs_data_block_read(block, (void*) buffer);
			buffer->blocks[index_2] = new_data_block;
			fs_data_block_write(block, (void*) buffer, FS_SYSTEM_BYTES_PER_BLOCK);
		} else {
			uint64_t index_1 = (block_count - table) / pow(indirect, 2);
			uint64_t index_2 = index_1 / indirect;
			uint64_t index_3 = ;
			fs_indirect_table_t* buffer;

			fs_data_block_read(icache->inode->first_indirect-table, (void*) buffer);
			fs_data_block_read(buffer->blocks[index_1], (void*) buffer);

			uint64_t block = buffer->blocks[index_2];

			fs_data_blocks_read(block, (void*) buffer);
			buffer->block[index_3] = new_data_block;
			fs_data_block_write(block, (void*) buffer, FS_SYSTEM_BYTES_PER_BLOCK);
		}


		inode->inode->block_count++;
	}
}

void fs_inode_del_data_blocks(fs_inode_cache_entry_t* inode, uint64_t count) {
	;
}
