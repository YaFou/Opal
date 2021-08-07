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

typedef Node* (*PrefixParseFunction)();
typedef Node* (*InfixParseFunction)(Node* left);

typedef struct {
    Precedence precedence;
    PrefixParseFunction prefix;
    InfixParseFunction infix;
} ParseRule;

Parser* parser;

static Node* binary(Node* left);
static Node* primary();
static Node* grouping();
static Node* unary();

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

static Token* peekAt(int index)
{
    return (Token*) VECTOR_GET(parser->tokens, index);
}

static Token* peek()
{
    return peekAt(parser->index);
}

static Token* peekNext()
{
    return peekAt(parser->index + 1);
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

static void advance()
{
    parser->index++;
}

static ParseRule* getRule(TokenType type)
{
    return &rules[type];
}

static void addErrorAtToken(Token* token, char* message)
{
    addErrorAt(parser->module, token->startIndex, token->endIndex, message);
}

static bool isAtEnd()
{
    return VECTOR_SIZE(parser->tokens) == parser->index + 1;
}

static Node* parsePrecedence(Precedence precedence)
{
    PrefixParseFunction prefixFunction = getRule(peek()->type)->prefix;

    if (prefixFunction == NULL) {
        addErrorAtToken(peek(), "Expect an expression.");

        return NULL;
    }

    Node* node = prefixFunction();

    if (isAtEnd()) {
        return node;
    }

    while (precedence <= getRule(peekNext()->type)->precedence) {
        advance();
        InfixParseFunction infixFunction = getRule(peek()->type)->infix;
        node = infixFunction(node);
        
        if (isAtEnd()) {
            return node;
        }
    }

    return node;
}

static Node* primary()
{
    Token* token = peek();

    switch (token->type) {
        case TOKEN_INTEGER: {
            Node* node = makeNode(NODE_INTEGER, token->startIndex, token->endIndex);
            node->children.integer = token->value.integer;

            return node;
        }
    }
}

static NodeType arithmeticOperation(Token* token)
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

static void consume(TokenType type, char* message)
{
    Token* token = peek();

    if (token->type != type) {
        addErrorAtToken(token, message);
    }
}

static Node* binary(Node* left)
{
    Token* token = peek();
    NodeType type = arithmeticOperation(token);
    advance();
    ParseRule* rule = getRule(token->type);
    Node* right = parsePrecedence(rule->precedence + 1);

    if (right == NULL) {
        return left;
    }

    Node* node = makeNode(type, left->startIndex, right->endIndex);
    node->children.left = left;
    node->children.right = right;

    return node;
}

static Node* expression()
{
    return parsePrecedence(PRECEDENCE_ASSIGNMENT);
}

static Node* unary()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* inner = parsePrecedence(PRECEDENCE_UNARY);
    Node* node = makeNode(NODE_NEGATE, startIndex, inner->endIndex);
    node->children.node = inner;

    return node;
}

static Node* grouping()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* node = expression();
    node->startIndex = startIndex;
    advance();
    consume(TOKEN_RIGHT_PAREN, "Expect \")\" after an expression.");
    node->endIndex = peek()->endIndex;

    return node;
}

Node* parse(Module* module, Vector* tokens)
{
    parser = newParser(module, tokens);
    Node* node = expression();
    free(parser);

    return node;
}

void freeNode(Node* node)
{
    switch (node->type) {
        case NODE_ADD:
        case NODE_SUBSTRACT:
        case NODE_MULTIPLY:
        case NODE_DIVIDE:
        case NODE_MODULO:
            freeNode(node->children.left);
            freeNode(node->children.right);
            break;
        case NODE_NEGATE:
            freeNode(node->children.node);
            break;
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
