#ifndef OPAL_VECTOR_H
#define OPAL_VECTOR_H

#define VECTOR_SIZE(vector) vector->size
#define VECTOR_GET(vector, index) vector->items[index]

typedef struct {
    void **items;
    int size;
    int capacity;
} Vector;

Vector* newVector();
void pushVector(Vector* vector, void *item);
void* popVector(Vector* vector);
void freeVector(Vector* vector);

#endif
