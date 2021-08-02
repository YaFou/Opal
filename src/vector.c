#include "vector.h"
#include "error.h"
#include <stdlib.h>

Vector* newVector()
{
    Vector* vector = malloc(sizeof(Vector));

    if (vector == NULL) {
        throwFailedAlloc();
    }

    vector->items = malloc(0);
    vector->size = 0;
    vector->capacity = 0;

    return vector;
}

void pushVector(Vector* vector, void *item)
{
    if (vector->size == vector->capacity) {
        vector->capacity = vector->capacity > 8 ? vector->capacity * 2 : 8;
        vector->items = realloc(vector->items, sizeof(void*) * vector->capacity);

        if (vector->items == NULL) {
            throwFailedAlloc();
        }
    }

    vector->items[vector->size++] = item;
}

void* popVector(Vector* vector)
{
    return vector->items[--vector->size];
}

void freeVector(Vector* vector)
{
    free(vector);
}
