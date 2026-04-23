#ifndef FS_BITMAP_H
#define FS_BITMAP_H

#include "stdbool.h"
#include "stdint.h"

#define FS_BITMAP_CACHE_SIZE 64

typedef struct {
 	uint64_t block[FS_BITMAP_CACHE_SIZE];
	uint64_t index;
	bool dirty;
} fs_bitmap_cache_t;

#endif
