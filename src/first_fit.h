#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include <stddef.h>

void init_heap();
void* my_malloc(size_t size);
void my_free(void* ptr);
void print_heap();
void reset_heap();

#endif
