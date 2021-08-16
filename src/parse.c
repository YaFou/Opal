#include "parse.h"
#include "util.h"
#include "scan.h"
#include "error.h"
#include <math.h>
#include <string.h>
#include "symbol.h"

#define TYPE_INTEGER "<integer>"
#define TYPE_BOOLEAN "<boolean>"
#define TYPE_NULL "<null>"

typedef struct {
    Module* module;
    int index;
    Vector* tokens;
    Environment* environment;
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
static Node* variable();

ParseRule rules[] = {
    [TOKEN_PLUS]                = {PRECEDENCE_TERM, NULL, binary},
    [TOKEN_MINUS]               = {PRECEDENCE_TERM, unary, binary},
    [TOKEN_STAR]                = {PRECEDENCE_FACTOR, NULL, binary},
    [TOKEN_SLASH]               = {PRECEDENCE_FACTOR, NULL, binary},
    [TOKEN_LEFT_PAREN]          = {PRECEDENCE_NONE, grouping, NULL},
    [TOKEN_RIGHT_PAREN]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LEFT_BRACE]          = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_RIGHT_BRACE]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_SEMILICON]           = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CIRCUMFLEX]          = {PRECEDENCE_POWER, NULL, binary},
    [TOKEN_FALSE]               = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_TRUE]                = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_NULL]                = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_EQUAL]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_DOUBLE_EQUAL]        = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_BANG_EQUAL]          = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_BANG]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LESS]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LESS_EQUAL]          = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_GREATER]             = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_GREATER_EQUAL]       = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_DOT]                 = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_COMMA]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_DOUBLE_AMPERSAND]    = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_DOUBLE_PIPE]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_QUESTION_MARK]       = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_PLUS_EQUAL]          = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_MINUS_EQUAL]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_STAR_EQUAL]          = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_SLASH_EQUAL]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_MODULO]              = {PRECEDENCE_FACTOR, NULL, binary},
    [TOKEN_MODULO_EQUAL]        = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LEFT_BRACKET]        = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_RIGHT_BRACKET]       = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_HASHTAG]             = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_COLON]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_INTEGER]             = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_STRING]              = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CHAR]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_FLOAT]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_IDENTIFIER]          = {PRECEDENCE_NONE, variable, NULL},
    [TOKEN_ABSTRACT]            = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_BREAK]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CLASS]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CONST]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CONTINUE]            = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_DO]                  = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_ELSE]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_ENUM]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_FOR]                 = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_IF]                  = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_INTERFACE]           = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LOOP]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_RETURN]              = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_VAR]                 = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_WHILE]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_EOF]                 = {PRECEDENCE_NONE, NULL, NULL},
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

static void back()
{
    parser->index--;
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
    parser->environment = newEnvironment();

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
    return VECTOR_SIZE(parser->tokens) <= parser->index + 1;
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
            node->valueType = TYPE_INTEGER;

            return node;
        }
        case TOKEN_FALSE: {
            Node* node = makeNode(NODE_BOOLEAN, token->startIndex, token->endIndex);
            node->children.boolean = false;
            node->valueType = TYPE_BOOLEAN;

            return node;
        }
        case TOKEN_TRUE: {
            Node* node = makeNode(NODE_BOOLEAN, token->startIndex, token->endIndex);
            node->children.boolean = true;
            node->valueType = TYPE_BOOLEAN;

            return node;
        }
        case TOKEN_NULL: {
            Node* node = makeNode(NODE_NULL, token->startIndex, token->endIndex);
            node->valueType = TYPE_NULL;

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

static Token* last()
{
    return VECTOR_LAST(parser->tokens);
}

static void consume(TokenType type, char* message)
{
    if (isAtEnd()) {
        addErrorAtToken(last(), message);
        back();

        return;
    }

    Token* token = peek();

    if (token->type != type) {
        addErrorAtToken(token, message);
        back();
    }
}

static bool isSameType(Node* node, char* type)
{
    return !strcmp(node->valueType, type);
}

static void checkTypes(Node* node)
{
    Node* left = node->children.left;
    Node* right = node->children.right;
    
    if (left->valueType == NULL || right->valueType == NULL) {
        return;
    }

    if (!isSameType(left, TYPE_INTEGER) || !isSameType(right, TYPE_INTEGER)) {
        addErrorAt(parser->module, node->startIndex, node->endIndex, "Types \"%s\" and \"%s\" are incompatible in a binary operation.", left->valueType, right->valueType);

        return;
    }

    node->valueType = TYPE_INTEGER;
}

static Node* binary(Node* left)
{
    Token* token = peek();
    printf("%d\n", left == NULL);
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
    checkTypes(node);

    return node;
}

static Node* expression()
{
    Node* node = parsePrecedence(PRECEDENCE_ASSIGNMENT);
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

static Node* declaration()
{
    Token* first = peek();
    advance();
    consume(TOKEN_IDENTIFIER, "Expect an identifier to declare a constant.");
    char* identifier = peek()->value.string;
    Token* last = peek();
    Node* value;

    if (peekNext()->type == TOKEN_EQUAL) {
        advance();
        advance();
        value = expression();

        if (value == NULL) {
            back();
        }

        last = peek();
    }

    Variable* variable = newEnvironmentVariable(parser->environment, identifier, value != NULL ? value->valueType : NULL);

    Node* node = makeNode(NODE_ASSIGNMENT, first->startIndex, last->endIndex);
    node->children.variableAssignment = variable;
    node->children.variableValue = value;

    return node;
}

static Node* statement()
{
    Token* token = peek();
    Node* node;

    switch (token->type) {
        case TOKEN_CONST:
            node = declaration();
            break;
        default:
            node = expression();
    }

    advance();
    consume(TOKEN_SEMILICON, "Expect \";\" after a statement.");

    if (node == NULL) {
        return NULL;
    }

    token = isAtEnd() ? last() : peek();
    node->endIndex = token->endIndex;

    return node;
}

static Node* variable()
{
    Token* token = peek();
    char* identifier = token->value.string;
    Variable* variable = getEnvironmentVariable(parser->environment, identifier);

    if (variable == NULL) {
        addErrorAtToken(token, format("Undefined variable \"%s\".", identifier));

        return NULL;
    }

    Node* node = makeNode(NODE_LOAD, token->startIndex, token->endIndex);
    node->children.variable = variable;
    node->valueType = variable->type;

    return node;
}

void freeParser()
{
    freeEnvironment(parser->environment);
    free(parser);
}

Node* parse(Module* module, Vector* tokens)
{
    parser = newParser(module, tokens);
    Vector* statements = newVector();

    while (!isAtEnd()) {
        pushVector(statements, statement());
        advance();
    }

    freeParser();
    Node* first = VECTOR_FIRST(statements);
    Node* last = VECTOR_LAST(statements);
    
    if (first == NULL || last == NULL) {
        return NULL;
    }

    Node* node = makeNode(NODE_STATEMENTS, first->startIndex, last->endIndex);
    node->children.nodes = statements;

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
        case NODE_POWER:
            freeNode(node->children.left);
            freeNode(node->children.right);
            break;
        case NODE_NEGATE:
            freeNode(node->children.node);
            break;
        case NODE_STATEMENTS: {
            for (VECTOR_EACH(node->children.nodes)) {
                Node* child = VECTOR_GET(node->children.nodes, i);
                freeNode(child);
            }

            freeVector(node->children.nodes);
            break;
        }
        case NODE_ASSIGNMENT: {
            if (node->children.variableValue != NULL) {
                freeNode(node->children.variableValue);
            }

            break;
        }
    }
    
    free(node);
}

void optimizeNode(Module* module, Node* node)
{
    switch (node->type) {
        case NODE_STATEMENTS: {
            for (VECTOR_EACH(node->children.nodes)) {
                Node* child = VECTOR_GET(node->children.nodes, i);
                optimizeNode(module, child);
            }
            
            return;
        }
        case NODE_NEGATE: {
            Node* inner = node->children.node;
            optimizeNode(module, inner);

            if (inner->type != NODE_INTEGER) {
                return;
            }

            node->children.integer = -inner->children.integer;
            node->type = NODE_INTEGER;

            return;
        }
        case NODE_ADD:
        case NODE_SUBSTRACT:
        case NODE_MULTIPLY:
        case NODE_DIVIDE:
        case NODE_MODULO:
        case NODE_POWER: {
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

            return;
        }
        case NODE_ASSIGNMENT: {
            Node* value = node->children.variableValue;

            if (value != NULL) {
                optimizeNode(module, value);
            }

            return;
        }
    }
}
