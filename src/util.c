#include "util.h"
#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stringbuilder.h"

static void* safeAlloc(void* pointer)
{
    if (pointer == NULL) {
        throwFailedAlloc();
    }

    return pointer;
}

void* safeMalloc(size_t size)
{
    return safeAlloc(malloc(size));
}

void* safeRealloc(void* block, size_t size)
{
    return safeAlloc(realloc(block, size));
}

char* format(char* format, ...)
{
    char buffer[2048];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return strdup(buffer);
}

bool isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char* repeatString(char* string, int times)
{
    StringBuilder* builder = newStringBuilder();

    for (int i = 0; i < times; i++) {
        appendStringBuilder(builder, string);
    }

    char* result = buildStringBuilder(builder);
    freeStringBuilder(builder);

    return result;
}
