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

	printf("allocating heap size: %d KiB\n", heap_remaining / 8192);

	for (uint8_t order = HEAP_MAX_ORDER; order >= 0; order--) {
		if (heap_remaining == 0) {
			break;
		}

		uint64_t block_size = 1ULL << (order + HEAP_BASE_BLOCK_ORDER);

		while (heap_remaining >= block_size &&
		       (block_start % block_size) == 0) {

			heap_add_block(order, (void*) block_start);

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

uint64_t heap_order_to_size(uint8_t order) {
	return 1ULL << (order + HEAP_BASE_BLOCK_ORDER);
}

void heap_add_block(uint8_t order, void* block_start) {
	if (order > HEAP_MAX_ORDER) {
		return;
	}

	heap_free_block_t* block = (heap_free_block_t*) block_start;
	block->next = (uint64_t*) heap_free_list[order];
	heap_free_list[order] = block;

	printf("added block order %d\n", order);
}

void heap_del_block(uint8_t order, void* block_start) {
	if (order > HEAP_MAX_ORDER) {
		return;
	}

	heap_free_block_t* block = (heap_free_block_t*) block_start;
	heap_free_block_t* current_block = heap_free_list[order];
	uint8_t list_index = 0;

	while (current_block->next != 0) {
		if (block != current_block) {
			current_block = (heap_free_block_t*) current_block->next;
			list_index++;
		}

		if (list_index == 0) {
			heap_free_list[0] = (heap_free_block_t*) block->next;
		} else if (block->next == 0) {
			heap_free_list[list_index - 1]->next = 0;
		} else {
			heap_free_list[list_index - 1]->next = (uint64_t*) heap_free_list[list_index + 1];
		}

		printf("deleting block order %d\n", order);

		return;
	}

	printf("deleting block fail\n");

	return;
}

void heap_split_block(uint8_t block_order, uint8_t target_order, void* block) {
	printf("splitting blocks from %d to %d\n", block_order, target_order);

	while (block_order > target_order) {
		heap_del_block(block_order, block);
		block_order--;
		heap_add_block(block_order, block);
	}
}

void* heap_alloc_block(uint8_t order) {
	if (!heap_free_list[order]) {
		return false;
	}

	heap_free_block_t* block = heap_free_list[order];
	heap_del_block(order, (void*) block);

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

	printf("allocating block order %d\n", original_order);

	for (; free_block_order < HEAP_MAX_ORDER + 1 - free_block_order; free_block_order++) {
		if (heap_free_list[free_block_order]) {
			break;
		}
	}

	printf("found block order %d\n", free_block_order);

	heap_split_block(free_block_order, original_order, heap_free_list[free_block_order]);
	void* ptr = heap_alloc_block(original_order);

	return ptr;
}

void heap_free(void* block) {
	uint8_t block_order = ((uint8_t*) block - sizeof(uint8_t))[0];

	//

	for (uint8_t order = block_order; order < HEAP_MAX_ORDER + 1; order++) {
		if (!heap_free_list[order]->next) {
			continue;
		}

		//
	}

	/* MERGING
	while (heap_free_list[order]) {
		if (heap_free_list[order] != block_start ^ heap_order_to_size(order)) {
			break;
		}

		heap_free_list[];
		order++;
	}
	*/
}
