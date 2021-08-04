#ifndef OPAL_SCAN_H
#define OPAL_SCAN_H

#include "module.h"
#include "vector.h"

typedef enum {
    // SIGNS
    TOKEN_PLUS,             // +
    TOKEN_MINUS,            // -
    TOKEN_STAR,             // *
    TOKEN_SLASH,            // /
    TOKEN_LEFT_PAREN,       // (
    TOKEN_RIGHT_PAREN,      // )
    TOKEN_LEFT_BRACE,       // {
    TOKEN_RIGHT_BRACE,      // }
    TOKEN_SEMILICON,        // ;
    TOKEN_EQUAL,            // =
    TOKEN_DOUBLE_EQUAL,     // ==
    TOKEN_BANG_EQUAL,       // !=
    TOKEN_BANG,             // !
    TOKEN_LESS,             // <
    TOKEN_LESS_EQUAL,       // <=
    TOKEN_GREATER,          // >
    TOKEN_GREATER_EQUAL,    // >=
    TOKEN_DOT,              // .
    TOKEN_COMMA,            // ,
    TOKEN_CIRCUMFLEX,       // ^
    TOKEN_DOUBLE_AMPERSAND, // &&
    TOKEN_DOUBLE_PIPE,      // ||
    TOKEN_QUESTION_MARK,    // ?
    TOKEN_PLUS_EQUAL,       // +=
    TOKEN_MINUS_EQUAL,      // -=
    TOKEN_STAR_EQUAL,       // *=
    TOKEN_SLASH_EQUAL,      // /=
    TOKEN_MODULO,           // %
    TOKEN_MODULO_EQUAL,     // %=
    TOKEN_LEFT_BRACKET,     // [
    TOKEN_RIGHT_BRACKET,    // ]
    TOKEN_HASHTAG,          // #

    // VALUES
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_FLOAT,
    TOKEN_IDENTIFIER,

    // KEYWORDS
    TOKEN_ABSTRACT,
    TOKEN_BREAK,
    TOKEN_CLASS,
    TOKEN_CONST,
    TOKEN_CONTINUE,
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_ENUM,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_INTERFACE,
    TOKEN_LOOP,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_WHILE,

    // OTHERS
    TOKEN_EOF,
    TOKEN_NONE
} TokenType;

typedef struct {
    TokenType type;
    union {
        int integer;
        char c;
        float _float;
        char* string;
    } value;

    Module* module;
    int startIndex;
    int endIndex;
} Token;

Vector* scan(Module* module);

#endif
