#include "util.h"
#include <stdio.h>
#include "memory.h"
#include "error.h"
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

// --- VECTOR ---

#define VEC_INITIAL_CAPACITY 8

Vector* newVector()
{
    Vector* vector = safeMalloc(sizeof(Vector));
    vector->items = safeMalloc(sizeof(void*) * VEC_INITIAL_CAPACITY);
    vector->size = 0;
    vector->capacity = VEC_INITIAL_CAPACITY;

    return vector;
}

void vectorPush(Vector* vector, void* item)
{
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->items = safeRealloc(vector->items, sizeof(void*) * vector->capacity);
    }

    vector->items[vector->size++] = item;
}

void freeVector(Vector* vector)
{
    free(vector->items);
    free(vector);
}

// --- STRING BUILDER ---

#define SB_INITIAL_CAPACITY 8

StringBuilder* newSB()
{
    StringBuilder* builder = safeMalloc(sizeof(StringBuilder));
    builder->string = safeMalloc(sizeof(char) * SB_INITIAL_CAPACITY);
    builder->length = 0;
    builder->capacity = SB_INITIAL_CAPACITY;

    return builder;
}

static void growSB(StringBuilder* builder, size_t length) {
    if (builder->length + length <= builder->capacity) {
        return;
    }

    while (builder->length + length > builder->capacity) {
        builder->capacity *= 2;
    }

    builder->string = safeRealloc(builder->string, builder->capacity);
}

void addSB(StringBuilder* builder, char c) {
    growSB(builder, 1);
    builder->string[builder->length++] = c;
}

void appendSB(StringBuilder* builder, char* string) {
    size_t length = strlen(string);
    growSB(builder, length);
    memcpy(builder->string + builder->length, string, length);
    builder->length += length;
}

char* buildSB(StringBuilder* builder) {
    addSB(builder, '\0');

    return strdup(builder->string);
}

void freeSB(StringBuilder* builder)
{
    free(builder->string);
    free(builder);
}

void clearSB(StringBuilder* builder)
{
    freeSB(builder);
    builder = newSB();
}

// --- OTHER ---

char* readFile(const char* path)
{
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        throwError(E001, path);
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = safeMalloc((fileSize + 1) * sizeof(char));
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

    if (bytesRead < fileSize) {
        throwError(E002, path);
    }

    buffer[bytesRead] = '\0';

    if (fclose(file)) {
        throwError(E003, path);
    }

    return buffer;
}

char* format(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    char buffer[BUFFER_SIZE];
    vsprintf(buffer, format, ap);
    va_end(ap);

    return strdup(buffer);
}

char* repeatString(char* string, size_t times)
{
    StringBuilder* builder = newSB();

    for (int i = 0; i < times; i++) {
        appendSB(builder, string);
    }

    char* result = buildSB(builder);
    freeSB(builder);

    return result;
}

bool tty()
{
    return isatty(fileno(stdout));
}
