#include "memory.h"
#include "error.h"
#include <stdlib.h>

static void* safePointer(void* pointer)
{
    if (pointer == NULL) {
        throwFatal(F001);
    }

    return pointer;
}

void* safeMalloc(size_t size)
{
    return safePointer(malloc(size));
}

void* safeRealloc(void* block, size_t size)
{
    return safePointer(realloc(block, size));
}
