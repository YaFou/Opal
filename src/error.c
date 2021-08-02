#include "error.h"
#include "vector.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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

void addError(char* message, ...)
{
    if (errors == NULL) {
        errors = newVector();
    }

    char buffer[2048];
    va_list args;
    va_start(args, message);
    vsprintf(buffer, message, args);
    va_end(args);
    pushVector(errors, buffer);
}

void throwErrors()
{
    for (int i = 0; i < VECTOR_SIZE(errors); i++) {
        fprintf(stderr, "[ERROR] ");
        fprintf(stderr, VECTOR_GET(errors, i));
        fprintf(stderr, "\n");
    }

    exit(1);
}
