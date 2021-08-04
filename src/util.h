#ifndef OPAL_UTIL_H
#define OPAL_UTIL_H

#include <stdlib.h>
#include <stdbool.h>

void* safeMalloc(size_t size);
void* safeRealloc(void* block, size_t size);
char* format(char* format, ...);
bool isWhitespace(char c);
char* repeatString(char* string, int times);

#endif
