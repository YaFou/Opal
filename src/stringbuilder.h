#ifndef OPAL_STRING_BUILDER_H
#define OPAL_STRING_BUILDER_H

typedef struct {
    char* string;
    int length;
    int capacity;
} StringBuilder;

StringBuilder* newStringBuilder();
void addStringBuilder(StringBuilder* builder, char c);
void appendStringBuilder(StringBuilder* builder, char* string);
char* buildStringBuilder(StringBuilder* builder);
void freeStringBuilder(StringBuilder* builder);
void clearStringBuilder(StringBuilder* builder);

#endif
