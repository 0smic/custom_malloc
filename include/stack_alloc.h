#ifndef CUSTOM_MALLOC_H
#define CUSTOM_MALLOC_H

void *heap_alloc(size_t size);
void heap_free(void *ptr);
void *heap_realloc(void *ptr, size_t size);
void heap_colle(void);

#endif

