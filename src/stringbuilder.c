#include "stringbuilder.h"
#include "util.h"
#include <string.h>

#define STRING_BUILDER_INITIAL_CAPACITY 8

StringBuilder* newStringBuilder() {
    StringBuilder *builder = safeMalloc(sizeof(StringBuilder));
    builder->string = safeMalloc(sizeof(char) * STRING_BUILDER_INITIAL_CAPACITY);
    builder->length = 0;
    builder->capacity = STRING_BUILDER_INITIAL_CAPACITY;

    return builder;
}

static void growStringBuilder(StringBuilder *builder, int length) {
    if (builder->length + length <= builder->capacity) {
        return;
    }

    while (builder->length + length > builder->capacity) {
        builder->capacity *= 2;
    }

    builder->string = safeRealloc(builder->string, builder->capacity);
}

void addStringBuilder(StringBuilder *builder, char c) {
    growStringBuilder(builder, 1);
    builder->string[builder->length++] = c;
}

void appendStringBuilderWithLength(StringBuilder *builder, char *string, int length) {
    growStringBuilder(builder, length);
    memcpy(builder->string + builder->length, string, length);
    builder->length += length;
}

void appendStringBuilder(StringBuilder *builder, char *string) {
    appendStringBuilderWithLength(builder, string, strlen(string));
}

char* buildStringBuilder(StringBuilder *builder) {
    addStringBuilder(builder, '\0');

    return builder->string;
}

void freeStringBuilder(StringBuilder* builder)
{
    free(builder->string);
    free(builder);
}

void clearStringBuilder(StringBuilder* builder)
{
    freeStringBuilder(builder);
    builder = newStringBuilder();
}
