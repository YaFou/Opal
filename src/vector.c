#include "vector.h"
#include "util.h"

#define VECTOR_INITIAL_CAPACITY 8

static void reallocateItems(Vector* vector)
{
}

Vector* newVector()
{
    Vector* vector = safeMalloc(sizeof(Vector));
    vector->items = safeMalloc(sizeof(void*) * VECTOR_INITIAL_CAPACITY);
    vector->size = 0;
    vector->capacity = VECTOR_INITIAL_CAPACITY;

    return vector;
}

void pushVector(Vector* vector, void *item)
{
    // printf("overflow: %d\n", vector->size == vector->capacity);
    if (vector->size == vector->capacity) {
        vector->capacity = vector->capacity >= 8 ? vector->capacity * 2 : 8;
        vector->items = safeRealloc(vector->items, sizeof(void*) * vector->capacity);
    }

    vector->items[vector->size++] = item;
}

void* popVector(Vector* vector)
{
    return vector->items[--vector->size];
}

void freeVector(Vector* vector)
{
    free(vector->items);
    free(vector);
}
