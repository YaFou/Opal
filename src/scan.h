#ifndef OPAL_SCAN_H
#define OPAL_SCAN_H

#include "module.h"
#include "util.h"

typedef enum {
    // SYMBOLS
    TOKEN_PLUS,             // +
    TOKEN_PLUS_EQUAL,       // +=
    TOKEN_DOUBLE_PLUS,      // ++
    TOKEN_MINUS,            // -
    TOKEN_MINUS_EQUAL,      // -=
    TOKEN_DOUBLE_MINUS,     // --
    TOKEN_STAR,             // *
    TOKEN_STAR_EQUAL,       // *=
    TOKEN_DOUBLE_STAR,      // **
    TOKEN_DOUBLE_STAR_EQUAL,// **=
    TOKEN_SLASH,            // /
    TOKEN_SLASH_EQUAL,      // /=
    TOKEN_MODULO,           // %
    TOKEN_MODULO_EQUAL,     // %=
    TOKEN_LEFT_PAREN,       // (
    TOKEN_RIGHT_PAREN,      // )
    TOKEN_LEFT_BRACE,       // {
    TOKEN_RIGHT_BRACE,      // }
    TOKEN_LEFT_BRACKET,     // [
    TOKEN_RIGHT_BRACKET,    // ]
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
    TOKEN_DOUBLE_AMPERSAND, // &&
    TOKEN_DOUBLE_PIPE,      // ||
    TOKEN_QUESTION_MARK,    // ?
    TOKEN_HASHTAG,          // #
    TOKEN_COLON,            // :
    TOKEN_AT_SYMBOL,        // @
    TOKEN_UNDERSCORE,       // _
    TOKEN_EOF,

    // LITERALS
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,

    // KEYWORDS
    TOKEN_ABSTRACT,
    TOKEN_BREAK,
    TOKEN_CLASS,
    TOKEN_CONST,
    TOKEN_CONSTRUCTOR,
    TOKEN_CONTINUE,
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_ENUM,
    TOKEN_FALSE,
    TOKEN_IF,
    TOKEN_INTERFACE,
    TOKEN_LOOP,
    TOKEN_MATCH,
    TOKEN_NEW,
    TOKEN_NULL,
    TOKEN_RETURN,
    TOKEN_STATIC,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,
} TokenType;

typedef struct {
    // HEADER
    Module* module;
    int startIndex;
    int endIndex;

    // BODY
    TokenType type;
    union {
        int integer;        // INTEGER
        float _float;       // FLOAT
        char _char;         // CHAR
        const char* string; // STRING | IDENTIFIER
    } value;
} Token;

Vector* scan(Module* module);
void freeTokens(Vector* tokens);

#endif
