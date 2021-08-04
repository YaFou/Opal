#include "debug.h"
#include "scan.h"
#include <stdio.h>

void debugTokens(Vector* tokens)
{
    for (VECTOR_EACH(tokens)) {
        Token* token = VECTOR_GET(tokens, i);

        switch (token->type) {
            case TOKEN_PLUS:
                printf("PLUS\n");
                break;
            case TOKEN_MINUS:
                printf("MINUS\n");
                break;
            case TOKEN_STAR:
                printf("STAR\n");
                break;
            case TOKEN_SLASH:
                printf("SLASH\n");
                break;
            case TOKEN_LEFT_PAREN:
                printf("LEFT_PAREN\n");
                break;
            case TOKEN_RIGHT_PAREN:
                printf("RIGHT_PAREN\n");
                break;
            case TOKEN_LEFT_BRACE:
                printf("LEFT_BRACE\n");
                break;
            case TOKEN_RIGHT_BRACE:
                printf("RIGHT_BRACE\n");
                break;
            case TOKEN_SEMILICON:
                printf("SEMILICON\n");
                break;
            case TOKEN_INTEGER:
                printf("INTEGER | %d\n", token->value.integer);
                break;
            case TOKEN_EOF:
                printf("EOF\n");
                break;
            default:
                printf("UNKNOWN\n");
        }
    }
}
