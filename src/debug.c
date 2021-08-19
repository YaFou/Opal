#include "debug.h"
#include "scan.h"
#include <stdio.h>

static const char* getTokenName(TokenType type)
{
    switch (type) {
        case TOKEN_PLUS:
            return "PLUS";
        case TOKEN_PLUS_EQUAL:
            return "PLUS_EQUAL";
        case TOKEN_DOUBLE_PLUS:
            return "DOUBLE_PLUS";
        case TOKEN_MINUS:
            return "MINUS";
        case TOKEN_MINUS_EQUAL:
            return "MINUS_EQUAL";
        case TOKEN_DOUBLE_MINUS:
            return "DOUBLE_MINUS";
        case TOKEN_STAR:
            return "STAR";
        case TOKEN_STAR_EQUAL:
            return "STAR_EQUAL";
        case TOKEN_DOUBLE_STAR:
            return "DOUBLE_STAR";
        case TOKEN_DOUBLE_STAR_EQUAL:
            return "DOUBLE_STAR_EQUAL";
        case TOKEN_SLASH:
            return "SLASH";
        case TOKEN_SLASH_EQUAL:
            return "SLASH_EQUAL";
        case TOKEN_MODULO:
            return "MODULO";
        case TOKEN_MODULO_EQUAL:
            return "MODULO_EQUAL";
        case TOKEN_LEFT_PAREN:
            return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN:
            return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACE:
            return "LEFT_BRACE";
        case TOKEN_RIGHT_BRACE:
            return "RIGHT_BRACE";
        case TOKEN_LEFT_BRACKET:
            return "LEFT_BRACKET";
        case TOKEN_RIGHT_BRACKET:
            return "RIGHT_BRACKET";
        case TOKEN_SEMILICON:
            return "SEMILICON";
        case TOKEN_EQUAL:
            return "EQUAL";
        case TOKEN_DOUBLE_EQUAL:
            return "DOUBLE_EQUAL";
        case TOKEN_BANG_EQUAL:
            return "BANG_EQUAL";
        case TOKEN_BANG:
            return "BANG";
        case TOKEN_LESS:
            return "LESS";
        case TOKEN_LESS_EQUAL:
            return "LESS_EQUAL";
        case TOKEN_GREATER:
            return "GREATER";
        case TOKEN_GREATER_EQUAL:
            return "GREATER_EQUAL";
        case TOKEN_DOT:
            return "DOT";
        case TOKEN_COMMA:
            return "COMMA";
        case TOKEN_DOUBLE_AMPERSAND:
            return "DOUBLE_AMPERSAND";
        case TOKEN_DOUBLE_PIPE:
            return "DOUBLE_PIPE";
        case TOKEN_QUESTION_MARK:
            return "QUESTION_MARK";
        case TOKEN_HASHTAG:
            return "HASHTAG";
        case TOKEN_COLON:
            return "COLON";
        case TOKEN_AT_SYMBOL:
            return "AT_SYMBOL";
        case TOKEN_UNDERSCORE:
            return "UNDERSCORE";
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_INTEGER:
            return "INTEGER";
        case TOKEN_FLOAT:
            return "FLOAT";
        case TOKEN_CHAR:
            return "CHAR";
        case TOKEN_STRING:
            return "STRING";
        case TOKEN_IDENTIFIER:
            return "IDENTIFIER";
        case TOKEN_ABSTRACT:
            return "ABSTRACT";
        case TOKEN_BREAK:
            return "BREAK";
        case TOKEN_CLASS:
            return "CLASS";
        case TOKEN_CONST:
            return "CONST";
        case TOKEN_CONSTRUCTOR:
            return "CONSTRUCTOR";
        case TOKEN_CONTINUE:
            return "CONTINUE";
        case TOKEN_DO:
            return "DO";
        case TOKEN_ELSE:
            return "ELSE";
        case TOKEN_ENUM:
            return "ENUM";
        case TOKEN_FALSE:
            return "FALSE";
        case TOKEN_IF:
            return "IF";
        case TOKEN_INTERFACE:
            return "INTERFACE";
        case TOKEN_LOOP:
            return "LOOP";
        case TOKEN_MATCH:
            return "MATCH";
        case TOKEN_NEW:
            return "NEW";
        case TOKEN_NULL:
            return "NULL";
        case TOKEN_RETURN:
            return "RETURN";
        case TOKEN_STATIC:
            return "STATIC";
        case TOKEN_TRUE:
            return "TRUE";
        case TOKEN_VAR:
            return "VAR";
        case TOKEN_WHILE:
            return "WHILE";
    }
}

void debugTokens(Vector* tokens)
{
    printf("--- TOKENS ---\n");

    VEC_EACH(tokens) {
        Token* token = VEC_EACH_GET(tokens);
        printf("%d -> %d | %s", token->startIndex, token->endIndex, getTokenName(token->type));

        switch (token->type) {
            case TOKEN_INTEGER:
                printf(" | %d", token->value.integer);
                break;
            case TOKEN_FLOAT:
                printf(" | %f", token->value._float);
                break;
            case TOKEN_CHAR:
                printf(" | %c", token->value._char);
                break;
            case TOKEN_STRING:
                printf(" | %s", token->value.string);
                break;
            case TOKEN_IDENTIFIER:
                printf(" | %s", token->value.string);
                break;
        }

        printf("\n");
    }
}
