#ifndef OPAL_VECTOR_H
#define OPAL_VECTOR_H

#define VECTOR_SIZE(vector) vector->size
#define VECTOR_GET(vector, index) vector->items[index]
#define VECTOR_EACH(vector) int i = 0; i < VECTOR_SIZE(vector); i++

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
