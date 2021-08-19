#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"
#include <string.h>

typedef struct {
    int line;
    int column;
} Position;

Vector* errors;

void throwFatal(const char* message)
{
    fprintf(stderr, "[FATAL] %s\n", message);
    exit(2);
}

static void addError(char* message)
{
    if (errors == NULL) {
        errors = newVector();
    }

    vectorPush(errors, message);
}

void throwError(const char* message, ...)
{
    va_list ap;
    va_start(ap, message);
    char buffer[BUFFER_SIZE];
    vsprintf(buffer, message, ap);
    va_end(ap);

    addError(buffer);
    throwErrors();
}

static void copyPosition(Position* source, Position* destination)
{
    destination->line = source->line;
    destination->column = source->column;
}

void addErrorAt(Module* module, int startIndex, int endIndex, const char* message, ...)
{
    va_list ap;
    va_start(ap, message);
    char buffer[BUFFER_SIZE];
    vsprintf(buffer, message, ap);
    va_end(ap);

    StringBuilder* builder = newSB();
    appendSB(builder, buffer);

    const char* source = module->source;
    int index = 0;
    Position start = {0, 0};
    Position end = {0, 0};
    Position current = {1, 1};
    Vector* lines = newVector();
    StringBuilder* lineBuilder = newSB();

    loop {
        if (index == startIndex) {
            copyPosition(&current, &start);
        }

        if (index == endIndex) {
            copyPosition(&current, &end);
        }

        char c = source[index++];
        current.column++;

        if (c == '\n') {
            current.line++;
            current.column = 1;

            if (end.line) {
                vectorPush(lines, buildSB(lineBuilder));
                freeSB(lineBuilder);

                break;
            }
            
            if (start.line) {
                vectorPush(lines, buildSB(lineBuilder));
                clearSB(lineBuilder);
            } else {
                clearSB(lineBuilder);
            }

            continue;
        }

        if (c == '\0') {
            if (start.line) {
                vectorPush(lines, buildSB(lineBuilder));

                if (!end.line) {
                    copyPosition(&current, &end);
                }
            }

            freeSB(lineBuilder);
            
            break;
        }

        addSB(lineBuilder, c);
    }

    appendSB(builder, format("\n--> %s - %d:%d", module->projectPath, start.line, start.column));
    int maxLineLength = strlen(format("%d", end.line));
    char* codeFormat = format("\n%%%dd | %%s\n", maxLineLength);

    VEC_EACH(lines) {
        char* line = VEC_EACH_GET(lines);
        int lineNumber = start.line + i;
        appendSB(builder, format(codeFormat, lineNumber, line));

        if (start.line == end.line) {
            appendSB(builder, format("%s | %s%s", repeatString(" ", maxLineLength), repeatString(" ", start.column - 1), repeatString("^", end.column - start.column)));
        }
    }

    freeVector(lines);
    addError(buildSB(builder));
    freeSB(builder);
}

void throwErrors()
{
    if (!errors->size) {
        return;
    }

    fprintf(stderr, "Compilation failed.\n%d %s occured.\n", errors->size, errors->size > 1 ? "errors have" : "error has");

    VEC_EACH(errors) {
        const char* message = VEC_EACH_GET(errors);
        fprintf(stderr, "\n[ERROR] %s\n", message);
    }

    exit(1);
}
