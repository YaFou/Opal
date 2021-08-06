#include "debug.h"
#include "scan.h"
#include <stdio.h>
#include <math.h>

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

int interpretNode(Node* node)
{
    int value;

    #define COMBINE(node, operator) interpretNode(node->children.left) operator interpretNode(node->children.right)
    switch (node->type) {
        case NODE_ADD:
            value = COMBINE(node, +);
            break;
        case NODE_SUBSTRACT:
            value = COMBINE(node, -);
            break;
        case NODE_MULTIPLY:
            value = COMBINE(node, *);
            break;
        case NODE_DIVIDE:
            value = COMBINE(node, /);
            break;
        case NODE_MODULO:
            value = COMBINE(node, %);
            break;
        case NODE_POWER:
            value = pow(interpretNode(node->children.left), interpretNode(node->children.right));
            break;
        case NODE_INTEGER:
            value = node->children.integer;
            break;
        case NODE_NEGATE:
            value = -node->children.node->children.integer;
            break;
    }
    #undef COMBINE

    return value;
}
