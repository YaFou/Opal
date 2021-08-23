#include "parse.h"
#include "memory.h"
#include "scan.h"
#include "error.h"

typedef struct {
    Module* module;
    Vector* tokens;
    int index;
} Parser;

typedef enum {
    PRECEDENCE_NONE,
    PRECEDENCE_ASSIGNMENT,  // = += -= *= **= /= %=
    PRECEDENCE_TERNARY,     // .. ? .. : ..
    PRECEDENCE_OR,          // ||
    PRECEDENCE_AND,         // &&
    PRECEDENCE_COMPARISON,  // == != < <= > >=
    PRECEDENCE_TERM,        // + -
    PRECEDENCE_FACTOR,      // * / %
    PRECEDENCE_UNARY,       // - !
    PRECEDENCE_POWER,       // **
    PRECEDENCE_CALL,        // . () []
} Precedence;

typedef Node* (*PrefixParseFunction)();
typedef Node* (*InfixParseFunction)(Node* left);

typedef struct {
    Precedence precedence;
    PrefixParseFunction prefix;
    InfixParseFunction infix;
} ParseRule;

Parser* parser;

static Node* blockStatement();
static Node* primary();
static Node* doubleOperation(Node* left);
static Node* unary();
static Node* grouping();
static Node* declaration();
static Node* load();
static Node* member(Node* left);
static Node* expression();
static Node* loopExpression();
static Node* whileExpression();
static Node* doWhileExpression();
static Node* ifExpression();
static Node* matchExpression();
static Node* ternaryExpression(Node* left);
static Node* returnExpression();
static Node* breakExpression();
static Node* continueExpression();
static Node* postfix(Node* left);

ParseRule rules[] = {
    [TOKEN_PLUS]                = {PRECEDENCE_TERM, unary, doubleOperation},
    [TOKEN_PLUS_EQUAL]          = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_DOUBLE_PLUS]         = {PRECEDENCE_CALL, unary, postfix},
    [TOKEN_MINUS]               = {PRECEDENCE_TERM, unary, doubleOperation},
    [TOKEN_MINUS_EQUAL]         = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_DOUBLE_MINUS]        = {PRECEDENCE_CALL, unary, postfix},
    [TOKEN_STAR]                = {PRECEDENCE_FACTOR, NULL, doubleOperation},
    [TOKEN_STAR_EQUAL]          = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_DOUBLE_STAR]         = {PRECEDENCE_POWER, NULL, doubleOperation},
    [TOKEN_DOUBLE_STAR_EQUAL]   = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_SLASH]               = {PRECEDENCE_FACTOR, NULL, doubleOperation},
    [TOKEN_SLASH_EQUAL]         = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_MODULO]              = {PRECEDENCE_FACTOR, NULL, doubleOperation},
    [TOKEN_MODULO_EQUAL]        = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_LEFT_PAREN]          = {PRECEDENCE_NONE, grouping, NULL},
    [TOKEN_RIGHT_PAREN]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LEFT_BRACE]          = {PRECEDENCE_NONE, blockStatement, NULL},
    [TOKEN_RIGHT_BRACE]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LEFT_BRACKET]        = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_RIGHT_BRACKET]       = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_SEMICOLON]           = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_EQUAL]               = {PRECEDENCE_ASSIGNMENT, NULL, doubleOperation},
    [TOKEN_DOUBLE_EQUAL]        = {PRECEDENCE_COMPARISON, NULL, doubleOperation},
    [TOKEN_BANG_EQUAL]          = {PRECEDENCE_COMPARISON, NULL, doubleOperation},
    [TOKEN_BANG]                = {PRECEDENCE_UNARY, unary, NULL},
    [TOKEN_LESS]                = {PRECEDENCE_COMPARISON, NULL, doubleOperation},
    [TOKEN_LESS_EQUAL]          = {PRECEDENCE_COMPARISON, NULL, doubleOperation},
    [TOKEN_GREATER]             = {PRECEDENCE_COMPARISON, NULL, doubleOperation},
    [TOKEN_GREATER_EQUAL]       = {PRECEDENCE_COMPARISON, NULL, doubleOperation},
    [TOKEN_DOT]                 = {PRECEDENCE_CALL, NULL, member},
    [TOKEN_COMMA]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_DOUBLE_AMPERSAND]    = {PRECEDENCE_AND, NULL, doubleOperation},
    [TOKEN_DOUBLE_PIPE]         = {PRECEDENCE_OR, NULL, doubleOperation},
    [TOKEN_QUESTION_MARK]       = {PRECEDENCE_TERNARY, NULL, ternaryExpression},
    [TOKEN_HASHTAG]             = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_COLON]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_AT_SYMBOL]           = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_UNDERSCORE]          = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_EOF]                 = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_INTEGER]             = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_FLOAT]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CHAR]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_STRING]              = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_IDENTIFIER]          = {PRECEDENCE_NONE, load, NULL},
    [TOKEN_ABSTRACT]            = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_BREAK]               = {PRECEDENCE_NONE, breakExpression, NULL},
    [TOKEN_CLASS]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CONST]               = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CONSTRUCTOR]         = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_CONTINUE]            = {PRECEDENCE_NONE, continueExpression, NULL},
    [TOKEN_DO]                  = {PRECEDENCE_ASSIGNMENT, doWhileExpression, NULL},
    [TOKEN_ELSE]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_ENUM]                = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_FALSE]               = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_IF]                  = {PRECEDENCE_ASSIGNMENT, ifExpression, NULL},
    [TOKEN_INTERFACE]           = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_LOOP]                = {PRECEDENCE_ASSIGNMENT, loopExpression, NULL},
    [TOKEN_MATCH]               = {PRECEDENCE_ASSIGNMENT, matchExpression, NULL},
    [TOKEN_NULL]                = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_RETURN]              = {PRECEDENCE_NONE, returnExpression, NULL},
    [TOKEN_STATIC]              = {PRECEDENCE_NONE, NULL, NULL},
    [TOKEN_TRUE]                = {PRECEDENCE_NONE, primary, NULL},
    [TOKEN_VAR]                 = {PRECEDENCE_ASSIGNMENT, declaration, NULL},
    [TOKEN_WHILE]               = {PRECEDENCE_ASSIGNMENT, whileExpression, NULL},
};

static ParseRule* getRule(TokenType_ type)
{
    return &rules[type];
}

static Parser* newParser(Module* module, Vector* tokens)
{
    Parser* parser = safeMalloc(sizeof(Parser));
    parser->module = module;
    parser->tokens = tokens;
    parser->index = 0;

    return parser;
}

static void freeParser(Parser* parser)
{
    free(parser);
}

static Token* peekAt(int index)
{
    return VEC_GET(parser->tokens, index);
}

static Token* peek()
{
    return peekAt(parser->index);
}

static bool check(TokenType_ type)
{
    return peek()->type == type;
}

static bool isAtEnd()
{
    return parser->tokens->size <= parser->index || check(TOKEN_EOF);
}

static Node* makeNode(NodeType type, int startIndex, int endIndex)
{
    Node* node = safeMalloc(sizeof(Node));
    node->module = parser->module;
    node->startIndex = startIndex;
    node->endIndex = endIndex;
    node->type = type;

    return node;
}

static void addErrorAtCurrent(const char* message)
{
    addErrorAt(parser->module, peek()->startIndex, peek()->endIndex, message);
}

static void addWarningAtCurrent(const char* message)
{
    addWarningAt(parser->module, peek()->startIndex, peek()->endIndex, message);
}

static void advance()
{
    parser->index++;
}

static void back()
{
    parser->index--;
}

static Node* parsePrecedence(Precedence precedence)
{
    PrefixParseFunction prefixFunction = getRule(peek()->type)->prefix;

    if (prefixFunction == NULL) {
        addErrorAtCurrent(E015);
        advance();

        return NULL;
    }

    Node* node = prefixFunction();

    if (!node) {
        return NULL;
    }

    if (isAtEnd()) {
        return node;
    }

    while (precedence <= getRule(peek()->type)->precedence) {
        InfixParseFunction infixFunction = getRule(peek()->type)->infix;
        node = infixFunction(node);

        if (!node) {
            return NULL;
        }
    }

    return node;
}

static Token* consume(TokenType_ type, const char* message)
{
    if (!check(type)) {
        addErrorAtCurrent(message);

        return NULL;
    }

    Token* token = peek();

    return token;
}

static Node* primary()
{
    Token* token = peek();
    Node* node = NULL;

    switch (token->type) {
        case TOKEN_INTEGER: {
            node = makeNode(NODE_INTEGER, token->startIndex, token->endIndex);
            node->children.integer = token->value.integer;

            break;
        }
        case TOKEN_FALSE: {
            node = makeNode(NODE_BOOLEAN, token->startIndex, token->endIndex);
            node->children.boolean = false;

            break;
        }
        case TOKEN_TRUE: {
            node = makeNode(NODE_BOOLEAN, token->startIndex, token->endIndex);
            node->children.boolean = true;

            break;
        }
        case TOKEN_NULL: {
            node = makeNode(NODE_NULL, token->startIndex, token->endIndex);
        }
    }

    advance();

    return node;
}

static Node* ternaryExpression(Node* left)
{
    advance();
    Node* thenExpression = parsePrecedence(PRECEDENCE_TERNARY + 1);

    if (!thenExpression) {
        back();

        return NULL;
    }

    if (!consume(TOKEN_COLON, E023)) {
        return NULL;
    }

    advance();

    Node* elseExpression = parsePrecedence(PRECEDENCE_TERNARY);

    if (!elseExpression) {
        back();

        return NULL;
    }

    Node* node = makeNode(NODE_IF, left->startIndex, elseExpression->endIndex);
    node->children.ifCondition = left;
    node->children.ifBody = thenExpression;
    node->children.elseBody = elseExpression;

    return node;
}

static Node* loopExpression()
{
    int startIndex = peek()->startIndex;
    advance();

    if (!consume(TOKEN_LEFT_BRACE, E013)) {
        return NULL;
    }

    Node* body = blockStatement();
    Node* node = makeNode(NODE_LOOP, startIndex, body->endIndex);
    node->children.node = body;

    return node;
}

static Node* whileExpression()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* condition = expression();

    if (!consume(TOKEN_LEFT_BRACE, E013)) {
        return NULL;
    }

    Node* body = blockStatement();
    Node* node = makeNode(NODE_WHILE, startIndex, body->endIndex);
    node->children.whileCondition = condition;
    node->children.whileBody = body;

    return node;
}

static Node* postfix(Node* left)
{
    NodeType type;

    switch (peek()->type) {
        case TOKEN_DOUBLE_PLUS:
            type = NODE_POST_INCREMENT;
            break;
        case TOKEN_DOUBLE_MINUS:
            type = NODE_POST_DECREMENT;
            break;
    }

    Node* node = makeNode(type, left->startIndex, peek()->endIndex);
    advance();
    node->children.node = left;

    return node;
}

static Node* breakExpression()
{
    Node* node = makeNode(NODE_BREAK, peek()->startIndex, peek()->endIndex);
    advance();

    return node;
}

static Node* continueExpression()
{
    Node* node = makeNode(NODE_CONTINUE, peek()->startIndex, peek()->endIndex);
    advance();

    return node;
}

static Node* doWhileExpression()
{
    int startIndex = peek()->startIndex;
    advance();

    if (!consume(TOKEN_LEFT_BRACE, E013)) {
        return NULL;
    }

    Node* body = blockStatement();

    if (!consume(TOKEN_WHILE, E019)) {
        return NULL;
    }

    advance();
    Node* condition = expression();

    if (!consume(TOKEN_SEMICOLON, E020)) {
        return NULL;
    }

    advance();
    Node* node = makeNode(NODE_DO_WHILE, startIndex, peek()->endIndex);
    node->children.whileCondition = condition;
    node->children.whileBody = body;

    return node;
}

static Node* ifExpression()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* condition = expression();

    if (!consume(TOKEN_LEFT_BRACE, E013)) {
        return NULL;
    }

    Node* body = blockStatement();
    Node* elseBody = NULL;

    if (check(TOKEN_ELSE)) {
        advance();

        if (check(TOKEN_IF)) {
            elseBody = ifExpression();
        } else {
            if (!consume(TOKEN_LEFT_BRACE, E013)) {
                return NULL;
            }

            elseBody = blockStatement();
        }
    }

    Node* node = makeNode(NODE_IF, startIndex, body->endIndex);
    node->children.ifCondition = condition;
    node->children.ifBody = body;
    node->children.elseBody = elseBody;

    return node;
}

static bool isBlockExpression()
{
    return check(TOKEN_LOOP) ||
        check(TOKEN_WHILE) ||
        check(TOKEN_DO) ||
        check(TOKEN_IF) ||
        check(TOKEN_MATCH) ||
        check(TOKEN_LEFT_BRACE);
}

static Node* matchArm()
{
    int startIndex = peek()->startIndex;
    Node* expr;
    NodeType type;

    if (!check(TOKEN_UNDERSCORE)) {
        expr = expression();
        type = NODE_MATCH_ARM;
    } else {
        type = NODE_MATCH_ARM_DEFAULT;
        advance();
    }

    if (!consume(TOKEN_COLON, E021)) {
        return NULL;
    }

    advance();
    bool isBlock = isBlockExpression();
    Node* body = expression();

    if (!body) {
        back();

        return NULL;
    }

    if (!isBlock) {
        if (!consume(TOKEN_COMMA, E022)) {
            return NULL;
        }

        advance();
    }

    Node* node = makeNode(type, startIndex, peekAt(parser->index - 1)->endIndex);

    if (type == NODE_MATCH_ARM) {
        node->children.matchArmExpression = expr;
        node->children.matchArmBody = body;
    } else {
        node->children.node = body;
    }

    return node;
}

static Node* matchExpression()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* value = expression();
    
    if (!consume(TOKEN_LEFT_BRACE, E013)) {
        return NULL;
    }

    Vector* arms = newVector();
    advance();

    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        Node* arm = matchArm();

        if (arm) {
            vectorPush(arms, arm);
        }
    }

    if (isAtEnd()) {
        addErrorAtCurrent(E012);

        return NULL;
    }

    Node* node = makeNode(NODE_MATCH, startIndex, peek()->endIndex);
    advance();
    node->children.matchValue = value;
    node->children.matchArms = arms;

    return node;
}

static Node* groupExpression()
{
    if (check(TOKEN_LOOP)) {
        return loopExpression();
    }

    if (check(TOKEN_WHILE)) {
        return whileExpression();
    }

    if (check(TOKEN_DO)) {
        return doWhileExpression();
    }

    if (check(TOKEN_IF)) {
        return ifExpression();
    }

    if (check(TOKEN_MATCH)) {
        return matchExpression();
    }

    return blockStatement();
}

static Node* inlineExpression()
{
    return parsePrecedence(PRECEDENCE_ASSIGNMENT);
}

static Node* expression()
{
    return inlineExpression();
}

static Node* grouping()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* node = expression();
    consume(TOKEN_RIGHT_PAREN, E012);

    return node;
}

static NodeType operator(TokenType_ type)
{
    switch (type) {
        case TOKEN_PLUS:
            return NODE_ADD;
        case TOKEN_MINUS:
            return NODE_SUBSTRACT;
        case TOKEN_STAR:
            return NODE_MULTIPLY;
        case TOKEN_DOUBLE_STAR:
            return NODE_POWER;
        case TOKEN_SLASH:
            return NODE_DIVIDE;
        case TOKEN_MODULO:
            return NODE_MODULE;
        case TOKEN_DOUBLE_AMPERSAND:
            return NODE_AND;
        case TOKEN_DOUBLE_PIPE:
            return NODE_OR;
        case TOKEN_DOUBLE_EQUAL:
            return NODE_EQUAL;
        case TOKEN_BANG_EQUAL:
            return NODE_NOT_EQUAL;
        case TOKEN_LESS:
            return NODE_LESS;
        case TOKEN_LESS_EQUAL:
            return NODE_LESS_EQUAL;
        case TOKEN_GREATER:
            return NODE_GREATER;
        case TOKEN_GREATER_EQUAL:
            return NODE_GREATER_EQUAL;
        case TOKEN_EQUAL:
            return NODE_ASSIGNMENT;
        case TOKEN_PLUS_EQUAL:
            return NODE_ADD_ASSIGNMENT;
        case TOKEN_MINUS_EQUAL:
            return NODE_SUBSTRACT_ASSIGNMENT;
        case TOKEN_STAR_EQUAL:
            return NODE_MULTIPLY_ASSIGNMENT;
        case TOKEN_DOUBLE_STAR_EQUAL:
            return NODE_POWER_ASSIGNMENT;
        case TOKEN_SLASH_EQUAL:
            return NODE_DIVIDE_ASSIGNMENT;
        case TOKEN_MODULO_EQUAL:
            return NODE_MODULO_ASSIGNMENT;
    }
}

static Node* load()
{
    Token* identifier = peek();
    Node* node = makeNode(NODE_LOAD, identifier->startIndex, identifier->endIndex);
    advance();

    return node;
}

static Node* unary()
{
    Token* operator = peek();
    NodeType type;
    Precedence precedence = PRECEDENCE_UNARY;

    switch (operator->type) {
        case TOKEN_PLUS:
            type = -1;
            break;
        case TOKEN_MINUS:
            type = NODE_NEGATE;
            break;
        case TOKEN_BANG:
            type = NODE_NOT;
            break;
        case TOKEN_DOUBLE_PLUS:
            type = NODE_PRE_INCREMENT;
            precedence = PRECEDENCE_CALL + 1;
            break;
        case TOKEN_DOUBLE_MINUS:
            type = NODE_PRE_DECREMENT;
            precedence = PRECEDENCE_CALL + 1;
            break;
    }

    advance();
    Node* inner = parsePrecedence(precedence);

    if (type == -1) {
        return inner;
    }

    Node* node = makeNode(type, operator->startIndex, operator->endIndex);
    node->children.node = inner;

    return node;
}

static Node* returnExpression()
{
    int startIndex = peek()->startIndex;
    advance();
    Node* value = expression();
    Node* node = makeNode(NODE_RETURN, startIndex, value->endIndex);
    node->children.node = value;

    return node;
}

static Node* doubleOperation(Node* left)
{
    Token* operatorToken = peek();
    NodeType type = operator(operatorToken->type);
    advance();
    ParseRule* rule = getRule(operatorToken->type);
    Node* right = parsePrecedence(rule->precedence + 1);

    if (right == NULL) {
        back();

        return left;
    }

    Node* node = makeNode(type, left->startIndex, right->endIndex);
    node->children.left = left;
    node->children.right = right;

    return node;
}

static Node* member(Node* left)
{
    advance();

    if (!consume(TOKEN_IDENTIFIER, E018)) {
        return NULL;
    }

    const char* member = peek()->value.string;
    Node* node = makeNode(NODE_MEMBER, left->startIndex, peek()->endIndex);
    advance();
    node->children.memberValue = left;
    node->children.memberName = member;

    return node;
}

static Node* declaration()
{
    int startIndex = peek()->startIndex;
    advance();

    if (!consume(TOKEN_IDENTIFIER, E016)) {
        advance();

        return NULL;
    }

    Token* identifier = peek();
    advance();

    if (!consume(TOKEN_EQUAL, E017)) {
        return NULL;
    }

    advance();
    Node* value = expression();

    if (!value) {
        return NULL;
    }

    Node* node = makeNode(NODE_VAR, startIndex, value->endIndex);
    node->children.varValue = value;

    return node;
}

static Node* statement()
{
    if (check(TOKEN_SEMICOLON)) {
        addWarningAtCurrent(W001);
        advance();

        return NULL;
    }

    if (isBlockExpression()) {
        return groupExpression();
    }

    Node* node = inlineExpression();

    if (!check(TOKEN_SEMICOLON)) {
        if (node) {
            Node* newNode = makeNode(NODE_IMPLICIT_RETURN, node->startIndex, node->endIndex);
            newNode->children.node = node;
            node = newNode;
        }
    } else {
        advance();
    }

    return node;
}

static Node* blockStatement()
{
    int startIndex = peek()->startIndex;
    advance();
    Vector* statements = newVector();

    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        Node* stat = statement();

        if (stat) {
            vectorPush(statements, stat);
        }
    }

    if (isAtEnd()) {
        addErrorAtCurrent(E012);

        return NULL;
    }

    Node* node = makeNode(NODE_STATEMENTS, startIndex, peek()->endIndex);
    node->children.nodes = statements;
    advance();

    return node;
}

static Node* function()
{
    Token* identifier = consume(TOKEN_IDENTIFIER, E011);
    advance();

    if (!identifier) {
        return NULL;
    }

    if (!consume(TOKEN_LEFT_PAREN, E010)) {
        return NULL;
    }

    advance();

    if (!consume(TOKEN_RIGHT_PAREN, E012)) {
        return NULL;
    }

    advance();

    if (!consume(TOKEN_LEFT_BRACE, E013)) {
        return NULL;
    }

    Node* body = blockStatement();

    if (!body) {
        return NULL;
    }

    Node* node = makeNode(NODE_FUNCTION, identifier->startIndex, body->endIndex);
    node->children.functionParameters = newVector();
    node->children.functionBody = body;

    return node;
}

static Node* rootDeclaration()
{
    return function();
}

static Node* module()
{
    Vector* declarations = newVector();

    while (!isAtEnd()) {
        vectorPush(declarations, rootDeclaration());
    }

    Node* first = VEC_FIRST(declarations);
    Node* last = VEC_LAST(declarations);

    if (!first || !last) {
        return NULL;
    }

    Node* node = makeNode(NODE_MODULE, first->startIndex, last->endIndex);
    node->children.nodes = declarations;
    
    return node;
}

Node* parse(Module* module_, Vector* tokens)
{
    parser = newParser(module_, tokens);
    Node* node = module();
    freeParser(parser);

    return node;
}

void freeNode(Node* node)
{
    switch (node->type) {
        case NODE_STATEMENTS:
        case NODE_MODULE: {
            VEC_EACH(node->children.nodes) {
                Node* child = VEC_EACH_GET(node->children.nodes);
                freeNode(child);
            }

            break;
        }
        case NODE_FUNCTION:
            freeVector(node->children.functionParameters);
            freeNode(node->children.functionBody);
            break;
        case NODE_VAR:
            freeNode(node->children.varValue);
            break;
        case NODE_ADD:
        case NODE_SUBSTRACT:
        case NODE_MULTIPLY:
        case NODE_DIVIDE:
        case NODE_MODULO:
        case NODE_POWER:
        case NODE_AND:
        case NODE_OR:
        case NODE_EQUAL:
        case NODE_NOT_EQUAL:
        case NODE_LESS:
        case NODE_LESS_EQUAL:
        case NODE_GREATER:
        case NODE_GREATER_EQUAL:
        case NODE_ASSIGNMENT:
        case NODE_ADD_ASSIGNMENT:
        case NODE_SUBSTRACT_ASSIGNMENT:
        case NODE_MULTIPLY_ASSIGNMENT:
        case NODE_DIVIDE_ASSIGNMENT:
        case NODE_MODULO_ASSIGNMENT:
        case NODE_POWER_ASSIGNMENT:
            freeNode(node->children.left);
            freeNode(node->children.right);
            break;
        case NODE_NEGATE:
        case NODE_NOT:
        case NODE_MATCH_ARM_DEFAULT:
        case NODE_RETURN:
        case NODE_IMPLICIT_RETURN:
        case NODE_PRE_INCREMENT:
        case NODE_PRE_DECREMENT:
        case NODE_POST_INCREMENT:
        case NODE_POST_DECREMENT:
            freeNode(node->children.node);
            break;
        case NODE_MATCH: {
            freeNode(node->children.matchValue);

            VEC_EACH(node->children.matchArms) {
                Node* child = VEC_EACH_GET(node->children.matchArms);
                freeNode(child);
            }

            freeVector(node->children.matchArms);
            break;
        }
        case NODE_MATCH_ARM:
            freeNode(node->children.matchArmExpression);
            freeNode(node->children.matchArmBody);
            break;
    }

    free(node);
}
