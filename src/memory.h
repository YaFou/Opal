#ifndef OPAL_MEMORY_H
#define OPAL_MEMORY_H

#include <stdlib.h>

void* safeMalloc(size_t size);
void* safeRealloc(void* block, size_t size);

#endif
