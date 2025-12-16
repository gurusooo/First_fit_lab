#include "first_fit.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_basic_allocation() {
    printf("\n=== Test 1: Basic Allocation ===\n");
    init_heap();
    
    void* p1 = my_malloc(100);
    assert(p1 != NULL);
    printf("Allocated 100 bytes at %p\n", p1);
    
    void* p2 = my_malloc(200);
    assert(p2 != NULL);
    printf("Allocated 200 bytes at %p\n", p2);
    
    print_heap();
    my_free(p1);
    my_free(p2);
}

void test_reuse_freed() {
    printf("\n=== Test 2: Reuse Freed Memory ===\n");
    reset_heap();
    
    void* p1 = my_malloc(300);
    void* p2 = my_malloc(400);
    
    my_free(p1);
    print_heap();
    
    void* p3 = my_malloc(250); /
    assert(p3 == p1); 
    printf("Reused freed block correctly\n");
    
    my_free(p2);
    my_free(p3);
}

void test_fragmentation() {
    printf("\n=== Test 3: Fragmentation Test ===\n");
    reset_heap();
    
    void* blocks[10];
    for (int i = 0; i < 5; i++) {
        blocks[i] = my_malloc(64);
    }
    
    for (int i = 0; i < 5; i += 2) {
        my_free(blocks[i]);
    }
    
    printf("After freeing every second block:\n");
    print_heap();
    
    void* large = my_malloc(256);
    if (large) {
        printf("Large allocation succeeded (defragmentation worked)\n");
        my_free(large);
    } else {
        printf("Large allocation failed (fragmentation present)\n");
    }
    
    for (int i = 1; i < 5; i += 2) {
        my_free(blocks[i]);
    }
}

void test_edge_cases() {
    printf("\n=== Test 4: Edge Cases ===\n");
    reset_heap();
    
    void* p0 = my_malloc(0);
    assert(p0 == NULL);
    printf("Zero allocation correctly rejected\n");
    
    void* huge = my_malloc(HEAP_SIZE * 2);
    assert(huge == NULL);
    printf("Oversize allocation correctly rejected\n");

    my_free(NULL);
    printf("Free(NULL) handled correctly\n");
    
    void* p = my_malloc(50);
    my_free(p);
    my_free(p); 
}

void test_stress() {
    printf("\n=== Test 5: Stress Test ===\n");
    reset_heap();
    
    void* pointers[100];
    int allocations = 0;
    
    for (int i = 0; i < 100; i++) {
        pointers[i] = my_malloc(1024);
        if (pointers[i]) allocations++;
        else break;
    }
    
    printf("Made %d allocations before OOM\n", allocations);
    print_heap();
    
    for (int i = 0; i < allocations; i++) {
        my_free(pointers[i]);
    }
    
    printf("After freeing all:\n");
    print_heap();
}

int main() {

    printf("First Fit Memory Allocator - Unit Tests\n");
    
    test_basic_allocation();
    test_reuse_freed();
    test_fragmentation();
    test_edge_cases();
    test_stress();
    
    printf("All tests completed successfully!\n");
    
    return 0;
}