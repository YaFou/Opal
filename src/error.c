#include "error.h"
#include "vector.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "util.h"
#include "stringbuilder.h"
#include <string.h>

Vector* errors;

void throwFailedAlloc()
{
    throwFatal("Failed to allocate memory.");
}

void throwFatal(char* message, ...)
{
    va_list args;
    va_start(args, message);
    fprintf(stderr, "[FATAL] ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(2);
}

static void addErrorNotFormat(char* message)
{
    if (errors == NULL) {
        errors = newVector();
    }

    pushVector(errors, message);
}

void addError(char* message, ...)
{
    char buffer[2048];
    va_list args;
    va_start(args, message);
    vsprintf(buffer, message, args);
    va_end(args);
    addErrorNotFormat(buffer);
}

void throwErrors()
{
    fprintf(stderr, "%d error%s occured.\n\n", VECTOR_SIZE(errors), VECTOR_SIZE(errors) > 1 ? "s have" : " has");

    for (VECTOR_EACH(errors)) {
        fprintf(stderr, "[ERROR] ");
        fprintf(stderr, VECTOR_GET(errors, i));
        fprintf(stderr, "\n");
    }

    exit(1);
}

void addErrorAt(Module* module, int startIndex, int endIndex, char* message, ...)
{
    char formattedMessage[2048];
    va_list args;
    va_start(args, message);
    vsprintf(formattedMessage, message, args);
    va_end(args);
    
    StringBuilder* builder = newStringBuilder();
    appendStringBuilder(builder, formattedMessage);

    const char* source = module->source;
    int index = 0;
    int startLine = 0;
    int startColumn = 0;
    int endLine = 0;
    int endColumn = 0;
    int lineIndex = 1;
    int columnIndex = 1;
    Vector* lines = newVector();
    StringBuilder* line = newStringBuilder();

    while (true) {
        if (index == startIndex) {
            break;
        }

        char c = source[index++];
        columnIndex++;

        if (c == '\n') {
            lineIndex++;
            columnIndex = 1;
            clearStringBuilder(line);
        }

        if (c == '\0') {
            freeVector(lines);
            freeStringBuilder(line);
            addError(buildStringBuilder(builder));
            freeStringBuilder(builder);

            return;
        }

        if (isWhitespace(c)) {
            continue;
        }

        addStringBuilder(line, c);
    }
    
    startLine = lineIndex;
    startColumn = columnIndex;
    bool end = false;

    while (true) {
        if (index == endIndex) {
            break;
        }

        char c = source[index++];
        columnIndex++;

        if (c == '\n') {
            lineIndex++;
            columnIndex++;
            pushVector(lines, line);
            line = newStringBuilder();
        }

        if (c == '\0') {
            pushVector(lines, line);
            end = true;

            break;
        }

        if (isWhitespace(c)) {
            continue;
        }

        addStringBuilder(line, c);
    }

    endLine = lineIndex;
    endColumn = columnIndex;
    
    if (!end) {
        while (true) {
            char c = source[index++];
            columnIndex++;

            if (c == '\n' || c == '\0') {
                pushVector(lines, line);

                break;
            }

            if (isWhitespace(c)) {
                continue;
            }

            addStringBuilder(line, c);
        }
    }

    appendStringBuilder(builder, format("\n--> %s - %d:%d\n", module->filename, startLine, startColumn));

    for (VECTOR_EACH(lines)) {
        StringBuilder* lineBuilder = VECTOR_GET(lines, i);
        char* line = buildStringBuilder(lineBuilder);
        int lineNumber = startLine + i;
        appendStringBuilder(builder, format("%d | %s\n", lineNumber, line));

        if (lineNumber == startLine && lineNumber == endLine) {
            appendStringBuilder(builder, format("  | %s%s\n", repeatString(" ", startColumn - 1), repeatString("^", endColumn - startColumn)));
        }

        freeStringBuilder(lineBuilder);
    }

    freeVector(lines);
    addErrorNotFormat(strdup(buildStringBuilder(builder)));
    freeStringBuilder(builder);
}

bool hasErrors()
{
    return errors != NULL && VECTOR_SIZE(errors);
}
