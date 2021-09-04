#ifndef OPAL_UTIL_H
#define OPAL_UTIL_H

#include <stdlib.h>
#include <stdbool.h>

#define loop while(1)
#define BUFFER_SIZE 2048

// --- CONSOLE FORMATING ---

#define TEXT_BOLD "\e[1m"
#define TEXT_FAINT "\e[2m"
#define TEXT_NORMAL "\e[22m"
#define TEXT_RESET "\e[0m"
// Colors
#define TEXT_RED "\e[31m"
#define TEXT_GREEN "\e[32m"
#define TEXT_YELLOW "\e[33m"
#define TEXT_BRIGHT_RED "\e[91m"
// Cursor
#define CURSOR_UP "\e[A"
#define CURSOR_ERASE_LINE "\e[K"
#define CURSOR_UP_ERASE CURSOR_UP CURSOR_ERASE_LINE

// --- VECTOR ---

#define VEC_GET(vector, index) vector->items[index]
#define VEC_FIRST(vector) VEC_GET(vector, 0)
#define VEC_LAST(vector) VEC_GET(vector, vector->size - 1)
#define VEC_EACH(vector) for (int i = 0; i < vector->size; i++)
#define VEC_EACH_GET(vector) VEC_GET(vector, i)

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
void sbAdd(StringBuilder* builder, char c);
char* buildSB(StringBuilder* builder);
void freeSB(StringBuilder* builder);
void sbAppend(StringBuilder* builder, char* string);
void clearSB(StringBuilder** builder);

// --- MAP ---

typedef struct {
    Vector* keys;
    Vector* values;
} Map;

Map* newMap();
void mapSet(Map* map, char* key, void* value);
void* mapGet(Map* map, char* key);
void freeMap(Map* map);
bool mapHas(Map* map, char* key);

// --- OTHER ---

char* readFile(char* path);
char* format(char* format, ...);
char* repeatString(char* string, size_t times);
bool tty();

#endif
