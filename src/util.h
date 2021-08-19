#ifndef OPAL_UTIL_H
#define OPAL_UTIL_H

#include <stdlib.h>

#define loop while(1)
#define BUFFER_SIZE 2048

// --- VECTOR ---

#define VEC_EACH(vector) for (int i = 0; i < vector->size; i++)
#define VEC_EACH_GET(vector) vector->items[i]

typedef struct {
    void** items;
    int size;
    int capacity;
} Vector;

Vector* newVector();
void vectorPush(Vector* vector, void* item);
void freeVector(Vector* vector);

// --- STRING BUIDLER ---

typedef struct {
    char* string;
    int length;
    int capacity;
} StringBuilder;

StringBuilder* newSB();
void addSB(StringBuilder* builder, char c);
char* buildSB(StringBuilder* builder);
void freeSB(StringBuilder* builder);
void appendSB(StringBuilder* builder, char* string);
void clearSB(StringBuilder* builder);

// --- OTHER ---

char* readFile(const char* path);
char* format(const char* format, ...);
char* repeatString(char* string, size_t times);

#endif
