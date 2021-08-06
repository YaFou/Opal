#include "parse.h"
#include "util.h"
#include "scan.h"
#include "error.h"
#include <math.h>

typedef struct {
    Module* module;
    int index;
    Vector* tokens;
} Parser;

typedef enum {
    PRECEDENCE_NONE,
    PRECEDENCE_ASSIGNMENT,  // =
    PRECEDENCE_OR,          // ||
    PRECEDENCE_AND,         // &&
    PRECEDENCE_EQUAL,       // == !=
    PRECEDENCE_COMPARISON,  // < <= > >=
    PRECEDENCE_TERM,        // + -
    PRECEDENCE_FACTOR,      // * / %
    PRECEDENCE_UNARY,       // ! -
    PRECEDENCE_POWER,       // ^
    PRECEDENCE_CALL,        // . ()
} Precedence;

typedef Node* (*PrefixParseFunction)(Parser* parser);
typedef Node* (*InfixParseFunction)(Parser* parser, Node* left);

typedef struct {
    Precedence precedence;
    PrefixParseFunction prefix;
    InfixParseFunction infix;
} ParseRule;

static Node* binary(Parser* parser, Node* left);
static Node* primary(Parser* parser);
static Node* grouping(Parser* parser);
static Node* unary(Parser* parser);

ParseRule rules[] = {
    [TOKEN_PLUS]                = {PRECEDENCE_TERM, NULL, binary},
    [TOKEN_MINUS]               = {PRECEDENCE_TERM, unary, binary},
    [TOKEN_STAR]                = {PRECEDENCE_FACTOR, NULL, binary},
    [TOKEN_SLASH]               = {PRECEDENCE_FACTOR, NULL, binary},
    [TOKEN_MODULO]              = {PRECEDENCE_FACTOR, NULL, binary},
    [TOKEN_LEFT_PAREN]          = {PRECEDENCE_NONE, grouping, NULL},
    [TOKEN_INTEGER]             = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_EOF]                 = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CIRCUMFLEX]          = {PRECEDENCE_POWER, NULL, binary},
};

static Token* peekAt(Parser* parser, int index)
{
    return (Token*) VECTOR_GET(parser->tokens, index);
}

static Token* peek(Parser* parser)
{
    return peekAt(parser, parser->index);
}

static Token* peekNext(Parser* parser)
{
    return peekAt(parser, parser->index + 1);
}

static Node* makeNode(NodeType type, int startIndex, int endIndex)
{
    Node* node = safeMalloc(sizeof(Node));
    node->type = type;
    node->startIndex = startIndex;
    node->endIndex = endIndex;

    return node;
}

static Parser* newParser(Module* module, Vector* tokens)
{
    Parser* parser = safeMalloc(sizeof(Parser));
    parser->module = module;
    parser->tokens = tokens;
    parser->index = 0;

    return parser;
}

static void advance(Parser* parser)
{
    parser->index++;
}

static ParseRule* getRule(TokenType type)
{
    return &rules[type];
}

static void addErrorAtToken(Parser* parser, Token* token, char* message)
{
    addErrorAt(parser->module, token->startIndex, token->endIndex, message);
}

static bool isAtEnd(Parser* parser)
{
    return VECTOR_SIZE(parser->tokens) == parser->index + 1;
}

static Node* parsePrecedence(Parser* parser, Precedence precedence)
{
    PrefixParseFunction prefixFunction = getRule(peek(parser)->type)->prefix;

    if (prefixFunction == NULL) {
        addErrorAtToken(parser, peek(parser), "Expect an expression.");

        return NULL;
    }

    Node* node = prefixFunction(parser);

    if (isAtEnd(parser)) {
        return node;
    }

    while (precedence <= getRule(peekNext(parser)->type)->precedence) {
        advance(parser);
        InfixParseFunction infixFunction = getRule(peek(parser)->type)->infix;
        node = infixFunction(parser, node);
        
        if (isAtEnd(parser)) {
            return node;
        }
    }

    return node;
}

static Node* primary(Parser* parser)
{
    Token* token = peek(parser);

    switch (token->type) {
        case TOKEN_INTEGER: {
            Node* node = makeNode(NODE_INTEGER, token->startIndex, token->endIndex);
            node->children.integer = token->value.integer;

            return node;
        }
    }
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
        case TOKEN_CIRCUMFLEX:
            return NODE_POWER;
    }
}

static void consume(Parser* parser, TokenType type, char* message)
{
    Token* token = peek(parser);

    if (token->type != type) {
        addErrorAtToken(parser, token, message);
    }
}

static Node* binary(Parser* parser, Node* left)
{
    Token* token = peek(parser);
    NodeType type = arithmeticOperation(parser, token);
    advance(parser);
    ParseRule* rule = getRule(token->type);
    Node* right = parsePrecedence(parser, rule->precedence + 1);

    if (right == NULL) {
        return left;
    }

    Node* node = makeNode(type, left->startIndex, right->endIndex);
    node->children.left = left;
    node->children.right = right;

    return node;
}

static Node* expression(Parser* parser)
{
    return parsePrecedence(parser, PRECEDENCE_ASSIGNMENT);
}

static Node* unary(Parser* parser)
{
    int startIndex = peek(parser)->startIndex;
    advance(parser);
    Node* inner = parsePrecedence(parser, PRECEDENCE_UNARY);
    Node* node = makeNode(NODE_NEGATE, startIndex, inner->endIndex);
    node->children.node = inner;

    return node;
}

static Node* grouping(Parser* parser)
{
    int startIndex = peek(parser)->startIndex;
    advance(parser);
    Node* node = expression(parser);
    node->startIndex = startIndex;
    advance(parser);
    consume(parser, TOKEN_RIGHT_PAREN, "Expect \")\" after an expression.");
    node->endIndex = peek(parser)->endIndex;

    return node;
}

Node* parse(Module* module, Vector* tokens)
{
    Parser* parser = newParser(module, tokens);
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

void optimizeNode(Module* module, Node* node)
{
    if (node->type == NODE_INTEGER) {
        return;
    }

    if (node->type == NODE_NEGATE) {
        Node* inner = node->children.node;
        optimizeNode(module, inner);

        if (inner->type != NODE_INTEGER) {
            return;
        }

        node->children.integer = -inner->children.integer;
        node->type = NODE_INTEGER;

        return;
    }

    Node* left = node->children.left;
    Node* right = node->children.right;

    optimizeNode(module, left);
    optimizeNode(module, right);

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
            if (right->children.integer == 0) {
                addErrorAt(module, right->startIndex, right->endIndex, "Can't divide per zero.");

                return;
            }

            value = COMBINE(/);
            break;
        case NODE_MODULO:
            if (right->children.integer == 0) {
                addErrorAt(module, right->startIndex, right->endIndex, "Can't modulo per zero.");

                return;
            }

            value = COMBINE(%);
            break;
        case NODE_POWER:
            value = pow(left->children.integer, right->children.integer);
            break;
    }
    #undef COMBINE

    freeNode(node->children.left);
    freeNode(node->children.right);
    node->type = NODE_INTEGER;
    node->children.integer = value;
}
