#include "pmm.h"
#include "string.h"
#include "vga_text.h"

Boot_Info* pmm_boot_info;

unsigned int pmm_memory_size = 0;
unsigned int pmm_used_blocks = 0;
unsigned int pmm_max_blocks = 0;
unsigned int* pmm_memory_map = 0;

void pmm_memory_map_set (unsigned int bit) {
	pmm_memory_map[bit / 32] |= (1 << (bit % 32));
}

void pmm_memory_map_unset (unsigned int bit) {
	pmm_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

unsigned char pmm_memory_map_test(unsigned int bit) {
	return pmm_memory_map[bit / 32] & (1 << (bit % 32));
}

int pmm_memory_map_find_free_block(void) {
	for (unsigned int i = 0; i < pmm_max_blocks; i++) {
		if (pmm_memory_map[i] == 0xFFFFFFFF) {
			continue;
		}

		for (unsigned int j = 0; j < 32; j++) {
			int bit = (i * 32) + j;

			if (!(pmm_memory_map_test(bit))) {
				return bit;
			}
		}
	}

	return -1;
}

int pmm_memory_map_find_free_region(unsigned int page_count) {
	unsigned int region_start = 0;
	unsigned char region_start_found = 0;
	unsigned int free_pages = 0;

	for (unsigned int i = 0; i < pmm_max_blocks; i++) {
		if (pmm_memory_map[i] == 0xFFFFFFFF) {
			continue;
		}

		region_start_found = 0;

		for (unsigned int j = 0; j < 32; j++) {
			int bit = (i * 32) + j;

			if (!pmm_memory_map_test(bit)) {
				region_start = bit;
				region_start_found = 1;
				break;
			}
		}

		if (!region_start_found) {
			continue;
		}

		free_pages = 0;

		for (unsigned int j = 0; j < page_count; j++) {
			if (pmm_memory_map_test(region_start + j)) {
				break;
			}

			free_pages++;
		}

		if (free_pages == page_count) {
			return region_start;
		}
	}

	return -1;
}

void pmm_init(void* boot_info_pointer) {
	pmm_boot_info = (Boot_Info*) boot_info_pointer;
	E820_Entry* memory_map = (E820_Entry*) pmm_boot_info->entries;

	unsigned long memory_size = 0;

	for (unsigned int i = 0; i < pmm_boot_info->entry_count; i++) {
		if (memory_map[i].type == 1) {
			memory_size += memory_map[i].length;
		}
	}

	pmm_memory_size = memory_size;
	// pmm_memory_map = bitmap;
	pmm_max_blocks = pmm_memory_size / PMM_BLOCK_SIZE;
	pmm_used_blocks = pmm_max_blocks;

	// memset(pmm_memory_map, 0xF, (pmm_max_blocks / PMM_BLOCKS_PER_BYTE));
}

void pmm_init_region(unsigned long base, unsigned int size) {
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	while (blocks > 0) {
		pmm_memory_map_unset(align++);
		pmm_used_blocks++;
		blocks--;
	}

	pmm_memory_map_set(0);
}

void pmm_deinit_region(unsigned long base, unsigned int size) {
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	while (blocks > 0) {
		pmm_memory_map_set(align++);
		pmm_used_blocks--;
		blocks--;
	}
}

void* pmm_alloc_block() {
	if (pmm_max_blocks - pmm_used_blocks <= 0) {
		return 0;
	}

	int frame = pmm_memory_map_find_free_block();

	if (frame == -1) {
		return 0;
	}

	pmm_memory_map_set(frame);

	unsigned long address = frame * PMM_BLOCK_SIZE;
	pmm_used_blocks++;

	return (void*) address;
}

void pmm_free_block(void* block_pointer) {
	unsigned long address = (unsigned long) block_pointer;
	int frame = address / PMM_BLOCK_SIZE;

	pmm_memory_map_unset(frame);

	pmm_used_blocks--;
}
