#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

#include "./algorithm/bucket.h"

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t number, size_t size);
void *realloc(void *ptr, size_t size);

#endif /* ! MALLOC_H */
