#include "first_fit.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define HEAP_SIZE 1024 * 1024  
static char heap[HEAP_SIZE];
static int initialized = 0;

typedef struct block_header {
    size_t size;               
    int is_free;                
    struct block_header* next; 
    struct block_header* prev;  
} block_header;

static block_header* head = NULL;

void init_heap() {
    if (initialized) return;
    
    head = (block_header*)heap;
    head->size = HEAP_SIZE - sizeof(block_header);
    head->is_free = 1;
    head->next = NULL;
    head->prev = NULL;
    
    initialized = 1;
    printf("Heap initialized with %d bytes\n", HEAP_SIZE);
}

static size_t align_size(size_t size) {
    return (size + 7) & ~7;
}

void* my_malloc(size_t size) {
    if (size == 0 || !initialized) return NULL;
    
    size = align_size(size);
    block_header* current = head;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size > size + sizeof(block_header) + 8) {
                block_header* new_block = (block_header*)((char*)current + sizeof(block_header) + size);
                new_block->size = current->size - size - sizeof(block_header);
                new_block->is_free = 1;
                new_block->next = current->next;
                new_block->prev = current;
                
                if (current->next) {
                    current->next->prev = new_block;
                }
                
                current->size = size;
                current->next = new_block;
            }
            
            current->is_free = 0;
            return (void*)((char*)current + sizeof(block_header));
        }
        current = current->next;
    }
    
    printf("Failed to allocate %zu bytes (no suitable block)\n", size);
    return NULL;
}

void my_free(void* ptr) {
    if (!ptr || !initialized) return;
    
    block_header* block = (block_header*)((char*)ptr - sizeof(block_header));
    
    if (block->is_free) {
        printf("Double free detected at %p\n", ptr);
        return;
    }
    
    block->is_free = 1;
    printf("Freed block at %p (size: %zu)\n", ptr, block->size);
    
    if (block->prev && block->prev->is_free) {
        block->prev->size += sizeof(block_header) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
        block = block->prev;
    }
    
    if (block->next && block->next->is_free) {
        block->size += sizeof(block_header) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
}

void print_heap() {
    if (!initialized) {
        printf("Heap not initialized\n");
        return;
    }
    
    block_header* current = head;
    int block_num = 0;
    size_t total_free = 0;
    size_t total_used = 0;
    
    printf("\n=== Heap Dump ===\n");
    while (current) {
        printf("Block %d: [%p] size=%zu, free=%d\n",
               block_num++, current, current->size, current->is_free);
        
        if (current->is_free) total_free += current->size;
        else total_used += current->size;
        
        current = current->next;
    }
    
    printf("Total used: %zu bytes\n", total_used);
    printf("Total free: %zu bytes\n", total_free);
    printf("Fragmentation: %.2f%%\n", 
           (total_free > 0 ? (float)total_free / (total_free + total_used) * 100 : 0));
}

void reset_heap() {
    init_heap();
    head->size = HEAP_SIZE - sizeof(block_header);
    head->is_free = 1;
    head->next = NULL;
    head->prev = NULL;
    printf("Heap reset\n");
}