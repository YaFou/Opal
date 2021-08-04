#include "parse.h"
#include "util.h"
#include "scan.h"
#include "error.h"

typedef struct {
    Module* module;
    int index;
    Vector* tokens;
} Parser;

static Parser* makeParser(Module* module, Vector* tokens)
{
    Parser* parser = safeMalloc(sizeof(Parser));
    parser->module = module;
    parser->index = 0;
    parser->tokens = tokens;

    return parser;
}

static void advance(Parser* parser)
{
    parser->index++;
}

static Token* getToken(Parser* parser)
{
    return (Token*) VECTOR_GET(parser->tokens, parser->index);
}

static Node* makeNode(NodeType type, int startIndex, int endIndex)
{
    Node* node = safeMalloc(sizeof(Node));
    node->type = type;
    node->startIndex = startIndex;
    node->endIndex = endIndex;

    return node;
}

static void addErrorAtToken(Parser* parser, Token* token, char* message)
{
    addErrorAt(parser->module, token->startIndex, token->endIndex, message);
}

static Node* primary(Parser* parser)
{
    Token* token = getToken(parser);
    Node* node = makeNode(NODE_INTEGER, token->startIndex, token->endIndex);

    if (token->type == TOKEN_INTEGER) {
        advance(parser);
        node->children.integer = token->value.integer;
    } else {
        addErrorAtToken(parser, token, "Expect an integer value.");
    }

    return node;
}

static NodeType arithmeticOperation(Parser* parser, Token* token)
{
    switch (token->type) {
        case TOKEN_PLUS:
            return NODE_ADD;
        case TOKEN_MINUS:
            return NODE_SUBSTRACT;
        case TOKEN_STAR:
            return NODE_MULTIPLY;
        case TOKEN_SLASH:
            return NODE_DIVIDE;
        case TOKEN_MODULO:
            return NODE_MODULO;
    }

    addErrorAtToken(parser, token, "Expect an arithmetic operation (+ - * / or %%%%).");
}

static Node* expression(Parser* parser)
{
    Node* left = primary(parser);
    Token* token = getToken(parser);
    advance(parser);

    if (token->type == TOKEN_EOF) {
        return left;
    }

    NodeType operation = arithmeticOperation(parser, token);
    Node* right = expression(parser);

    Node* node = makeNode(operation, left->startIndex, right->endIndex);
    node->children.left = left;
    node->children.right = right;

    return node;
}

Node* parse(Module* module, Vector* tokens)
{
    Parser* parser = makeParser(module, tokens);
    Node* node = expression(parser);
    free(parser);

    return node;
}

void freeNode(Node* node)
{
    if (node->type != NODE_INTEGER) {
        freeNode(node->children.left);
        freeNode(node->children.right);
    }
    
    free(node);
}

void optimizeNode(Node* node)
{
    if (node->type == NODE_INTEGER) {
        return;
    }

    Node* left = node->children.left;
    Node* right = node->children.right;

    optimizeNode(left);
    optimizeNode(right);

    if (left->type != NODE_INTEGER || right->type != NODE_INTEGER) {
        return;
    }

    int value;

    #define COMBINE(operator) left->children.integer operator right->children.integer
    switch (node->type) {
        case NODE_ADD:
            value = COMBINE(+);
            break;
        case NODE_SUBSTRACT:
            value = COMBINE(-);
            break;
        case NODE_MULTIPLY:
            value = COMBINE(*);
            break;
        case NODE_DIVIDE:
            value = COMBINE(/);
            break;
        case NODE_MODULO:
            value = COMBINE(%);
            break;
    }
    #undef COMBINE

    freeNode(node->children.left);
    freeNode(node->children.right);
    node->type = NODE_INTEGER;
    node->children.integer = value;
}
