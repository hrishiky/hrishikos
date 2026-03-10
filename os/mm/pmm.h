#ifndef PMM_H
#define PMM_H

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

typedef struct __attribute__((packed)) {
        unsigned long base;
        unsigned long length;
        unsigned int type;
        unsigned int acpi_null;
} E820_Entry;

typedef struct __attribute__((packed)) {
        unsigned int entry_count;
        unsigned long entries;
} Boot_Info;

void pmm_memory_map_set (unsigned int bit);
void pmm_memory_map_unset (unsigned int bit);
unsigned char pmm_memory_map_test(unsigned int bit);
int pmm_memory_map_find_free_block(void);
int pmm_memory_map_find_free_region(unsigned int page_count);
void pmm_init(void* boot_info_pointer);
void pmm_init_region(unsigned long base, unsigned int size);
void pmm_deinit_region(unsigned long base, unsigned int size);
void* pmm_alloc_block();
void pmm_free_block(void* block_pointer);

#endif
