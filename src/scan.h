#ifndef OPAL_SCAN_H
#define OPAL_SCAN_H

#include "vector.h"
#include "module.h"

typedef enum {
    // SIGNS
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_LEFT_PAREN,   // (
    TOKEN_RIGHT_PAREN,  // )
    TOKEN_LEFT_BRACE,   // {
    TOKEN_RIGHT_BRACE,  // }
    TOKEN_SEMILICON,    // ;

    // VALUES
    TOKEN_INTEGER,

    // OTHERS
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    union {
        int integer;
    } value;

    Module* module;
    int startIndex;
    int endIndex;
} Token;

Vector* scan(Module* module);

#endif
