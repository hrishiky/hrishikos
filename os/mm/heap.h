#ifndef HEAP_H
#define HEAP_H

#include "stdint.h"
#include "stdbool.h"

#define HEAP_SIZE 0x1000000
#define HEAP_START 0xFFFF800000400000
#define HEAP_END (HEAP_START + HEAP_SIZE)
#define HEAP_MAX_ORDER 12
#define HEAP_BASE_BLOCK_SIZE 4096
#define HEAP_BASE_BLOCK_ORDER 12

typedef struct {
	uint64_t* next;
} heap_free_block_t;

typedef struct {
	uint8_t order;
} heap_header_t;

void heap_init(void);
uint8_t heap_size_to_order(uint64_t size);
bool heap_split_block(uint8_t free_block_order, uint8_t target_order);
void* heap_alloc_block(uint8_t order);
void* heap_alloc(uint64_t size);

#endif
