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
Vector* warnings;

void throwFatal(const char* message)
{
    fprintf(
        stderr,
        tty() ?
        TEXT_BOLD TEXT_BRIGHT_RED "[FATAL]" TEXT_NORMAL " %s\n" TEXT_RESET :
        "[FATAL] %s\n",
        message
    );

    exit(2);
}

static void addError(char* message)
{
    if (errors == NULL) {
        errors = newVector();
    }

    vectorPush(errors, message);
}

static void addWarning(char* message)
{
    if (warnings == NULL) {
        warnings = newVector();
    }

    vectorPush(warnings, message);
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

static char* generateCode(Module* module, int startIndex, int endIndex, char* message, const char* color)
{
    StringBuilder* builder = newSB();
    appendSB(builder, message);

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

        if (c == '\r') {
            continue;
        }

        addSB(lineBuilder, c);
    }

    appendSB(builder, format(tty() ? TEXT_RESET TEXT_FAINT "\n--> %s - %d:%d" : "\n--> %s - %d:%d", module->projectPath, start.line, start.column));
    int maxLineLength = strlen(format("%d", end.line));
    char* codeFormat = format(tty() ? TEXT_RESET "\n%%%dd | %%s\n" : "\n%%%dd | %%s\n", maxLineLength);

    VEC_EACH(lines) {
        char* line = VEC_EACH_GET(lines);
        int lineNumber = start.line + i;
        appendSB(builder, format(codeFormat, lineNumber, line));

        if (start.line == end.line) {
            char* footer = tty() ?
                format("%s | %s%s%s", repeatString(" ", maxLineLength), color, repeatString(" ", start.column - 1), repeatString("^", end.column - start.column)) :
                format("%s | %s%s", repeatString(" ", maxLineLength), repeatString(" ", start.column - 1), repeatString("^", end.column - start.column));

            appendSB(builder, footer);
        }

        free(line);
    }

    freeVector(lines);
    char* code = buildSB(builder);
    freeSB(builder);

    return code;
}

void addErrorAt(Module* module, int startIndex, int endIndex, const char* message, ...)
{
    va_list ap;
    va_start(ap, message);
    char buffer[BUFFER_SIZE];
    vsprintf(buffer, message, ap);
    va_end(ap);

    addError(generateCode(module, startIndex, endIndex, buffer, TEXT_RED));
}

void throwErrors()
{
    fprintf(stderr, tty() ? TEXT_RED "=== %d %s occured.\n" : "=== %d %s occured.\n", errors->size, errors->size > 1 ? "errors have" : "error has");

    VEC_EACH(errors) {
        const char* message = VEC_EACH_GET(errors);
        fprintf(stderr, tty() ? TEXT_BOLD TEXT_RED "\n[ERROR]" TEXT_NORMAL " %s\n" TEXT_RESET : "\n[ERROR] %s\n", message);
    }

    if (warnings && warnings->size) {
        fprintf(stderr, "\n");
    }

    throwWarnings();
    exit(1);
}

bool hasErrors()
{
    return errors && errors->size;
}

void addWarningAt(Module* module, int startIndex, int endIndex, const char* message, ...)
{
    va_list ap;
    va_start(ap, message);
    char buffer[BUFFER_SIZE];
    vsprintf(buffer, message, ap);
    va_end(ap);
    
    addWarning(generateCode(module, startIndex, endIndex, buffer, TEXT_YELLOW));
}

void throwWarnings()
{
    if (!warnings || !warnings->size) {
        return;
    }

    fprintf(stderr, tty() ? TEXT_YELLOW "=== %d %s occured.\n" : "=== %d %s occured.\n", warnings->size, warnings->size > 1 ? "warnings have" : "warning has");

    VEC_EACH(warnings) {
        const char* message = VEC_EACH_GET(warnings);
        fprintf(stderr, tty() ? TEXT_BOLD TEXT_YELLOW "\n[WARNING]" TEXT_NORMAL " %s\n" TEXT_RESET : "\n[WARNING] %s\n", message);
    }
}
