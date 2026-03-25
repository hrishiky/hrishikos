#include "heap.h"

#include "pmm.h"
#include "vmm.h"
#include "stdint.h"
#include "stdio.h"

heap_free_block_t* heap_free_list[HEAP_MAX_ORDER + 1];

void heap_init(void) {
	printf("heap init start\n");

	uint64_t block_start = HEAP_START;
	uint64_t heap_remaining = HEAP_SIZE;

	for (uint8_t order = HEAP_MAX_ORDER; order >= 0; order--) {
		if (heap_remaining == 0) {
			break;
		}

		uint64_t block_size = 1ULL << (order + HEAP_BASE_BLOCK_ORDER);

		while (heap_remaining >= block_size &&
		       (block_start % block_size) == 0) {

			heap_free_block_t* block = (heap_free_block_t*) block_start;
			block->next = (uint64_t*) &heap_free_list[order];
			heap_free_list[order] = block;

			block_start += block_size;
			heap_remaining -= block_size;
			printf("block size assigned: %d KiB; heap remaining: %d KiB\n", block_size / 8192, heap_remaining / 8192);
		}
	}

	printf("heap init done\n\n");
}

uint8_t heap_size_to_order(uint64_t size) {
	uint8_t order = 0;
	size += sizeof(heap_header_t);
	uint64_t block_size = HEAP_BASE_BLOCK_SIZE;

	while (block_size <= size &&
	       order < HEAP_MAX_ORDER) {
		order++;
		block_size <<= 1;
	}

	return order;
}

bool heap_split_block(uint8_t free_block_order, uint8_t target_order) {
	if (!heap_free_list[free_block_order]) {
		return false;
	}

	while (free_block_order >= target_order) {
		heap_free_block_t* head_block = (heap_free_block_t*) heap_free_list[free_block_order]->next;
		heap_free_list[free_block_order] = head_block;

		heap_free_block_t* split_block_1 = (heap_free_block_t*) &heap_free_list[free_block_order];
		split_block_1->next = (uint64_t*) &heap_free_list[free_block_order - 1];
		heap_free_list[free_block_order - 1] = split_block_1;

		heap_free_block_t* split_block_2 = (heap_free_block_t*) &heap_free_list[free_block_order] + (1ULL << (free_block_order + HEAP_BASE_BLOCK_ORDER));
		split_block_2->next = (uint64_t*) &heap_free_list[free_block_order - 1];
		heap_free_list[free_block_order - 1] = split_block_2;

		free_block_order--;
	}

	return true;
}

void* heap_alloc_block(uint8_t order) {
	if (!heap_free_list[order]) {
		return false;
	}

	heap_free_block_t* block = heap_free_list[order];

	heap_free_block_t* head_block = (heap_free_block_t*) block->next;
	heap_free_list[order] = head_block;

	for (void* virtual_address = (void*) block; virtual_address < (void*) block + ((1ULL << (order + HEAP_BASE_BLOCK_ORDER) + PMM_BLOCK_SIZE) / PMM_BLOCK_SIZE); virtual_address += PMM_BLOCK_SIZE) {
		vmm_map_page(pmm_alloc_block(), virtual_address, (page_table_t*) vmm_get_cr3());
	}

	heap_header_t* header = (heap_header_t*) block;
	header->order = order;

	return (void*) block + sizeof(heap_header_t);
}

void* heap_alloc(uint64_t size) {
	uint8_t original_order = heap_size_to_order(size);
	uint8_t free_block_order = original_order;

	for (; free_block_order < HEAP_MAX_ORDER + 1 - free_block_order; free_block_order++) {
		if (heap_free_list[free_block_order]) {
			break;
		}
	}

	if (!heap_split_block(free_block_order, original_order)) {
		return 0;
	}

	return heap_alloc_block(original_order);
}
