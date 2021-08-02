#ifndef OPAL_TOKENIZER_H
#define OPAL_TOKENIZER_H

#include "vector.h"
#include "module.h"

typedef enum {
    PLUS,
    MINUS,
    STAR,
    SLASH,
    INTEGER
} TokenType;

typedef struct {
    TokenType type;
    union {
        int integer;
    } value;

    char* module;
    int start;
    int end;
} Token;

Vector* tokenize(Module* module);

#endif
