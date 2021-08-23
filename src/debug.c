#include "debug.h"
#include "scan.h"
#include <stdio.h>

static const char* getTokenName(TokenType_ type)
{
    switch (type) {
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_PLUS_EQUAL: return "PLUS_EQUAL";
        case TOKEN_DOUBLE_PLUS: return "DOUBLE_PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MINUS_EQUAL: return "MINUS_EQUAL";
        case TOKEN_DOUBLE_MINUS: return "DOUBLE_MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_STAR_EQUAL: return "STAR_EQUAL";
        case TOKEN_DOUBLE_STAR: return "DOUBLE_STAR";
        case TOKEN_DOUBLE_STAR_EQUAL: return "DOUBLE_STAR_EQUAL";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_SLASH_EQUAL: return "SLASH_EQUAL";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_MODULO_EQUAL: return "MODULO_EQUAL";
        case TOKEN_LEFT_PAREN: return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACE: return "LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "RIGHT_BRACE";
        case TOKEN_LEFT_BRACKET: return "LEFT_BRACKET";
        case TOKEN_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TOKEN_SEMICOLON: return "SEMILICON";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_DOUBLE_EQUAL: return "DOUBLE_EQUAL";
        case TOKEN_BANG_EQUAL: return "BANG_EQUAL";
        case TOKEN_BANG: return "BANG";
        case TOKEN_LESS: return "LESS";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_DOT: return "DOT";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOUBLE_AMPERSAND: return "DOUBLE_AMPERSAND";
        case TOKEN_DOUBLE_PIPE: return "DOUBLE_PIPE";
        case TOKEN_QUESTION_MARK: return "QUESTION_MARK";
        case TOKEN_HASHTAG: return "HASHTAG";
        case TOKEN_COLON: return "COLON";
        case TOKEN_AT_SYMBOL: return "AT_SYMBOL";
        case TOKEN_UNDERSCORE: return "UNDERSCORE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_ABSTRACT: return "ABSTRACT";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CLASS: return "CLASS";
        case TOKEN_CONST: return "CONST";
        case TOKEN_CONSTRUCTOR: return "CONSTRUCTOR";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_DO: return "DO";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_ENUM: return "ENUM";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_IF: return "IF";
        case TOKEN_INTERFACE: return "INTERFACE";
        case TOKEN_LOOP: return "LOOP";
        case TOKEN_MATCH: return "MATCH";
        case TOKEN_NULL: return "NULL";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_STATIC: return "STATIC";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_VAR: return "VAR";
        case TOKEN_WHILE: return "WHILE";
        default: return format("UNKNOWN [%d]", type);
    }
}

void debugTokens(Vector* tokens)
{
    printf("--- TOKENS ---\n");

    VEC_EACH(tokens) {
        Token* token = VEC_EACH_GET(tokens);
        printf("%3d | %.3d -> %.3d | %-15s", i, token->startIndex, token->endIndex, getTokenName(token->type));

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

    printf("\n\n");
}

int spacesNumber;
char* spaces;

static void indent()
{
    spacesNumber += 2;
    spaces = repeatString(" ", spacesNumber);
}

static void outdent()
{
    spacesNumber -= 2;
    spaces = repeatString(" ", spacesNumber);
}

static const char* getNodeName(NodeType type)
{
    switch (type) {
        case NODE_FUNCTION: return "FUNCTION";
        case NODE_MODULE: return "MODULE";
        case NODE_VAR: return "VAR";
        case NODE_ADD: return "ADD";
        case NODE_SUBSTRACT: return "SUBSTRACT";
        case NODE_MULTIPLY: return "MULTIPLY";
        case NODE_DIVIDE: return "DIVIDE";
        case NODE_MODULO: return "MODULO";
        case NODE_POWER: return "POWER";
        case NODE_AND: return "AND";
        case NODE_OR: return "OR";
        case NODE_NEGATE: return "NEGATE";
        case NODE_NOT: return "NOT";
        case NODE_EQUAL: return "EQUAL";
        case NODE_NOT_EQUAL: return "NOT_EQUAL";
        case NODE_LESS: return "LESS";
        case NODE_LESS_EQUAL: return "LESS_EQUAL";
        case NODE_GREATER: return "GREATER";
        case NODE_GREATER_EQUAL: return "GREATER_EQUAL";
        case NODE_ADD_ASSIGNMENT: return "ADD_ASSIGNMENT";
        case NODE_SUBSTRACT_ASSIGNMENT: return "SUBSTRACT_ASSIGNMENT";
        case NODE_MULTIPLY_ASSIGNMENT: return "MULTIPLY_ASSIGNMENT";
        case NODE_DIVIDE_ASSIGNMENT: return "DIVIDE_ASSIGNMENT";
        case NODE_MODULO_ASSIGNMENT: return "MODULO_ASSIGNMENT";
        case NODE_POWER_ASSIGNMENT: return "POWER_ASSIGNMENT";
        case NODE_LOAD: return "LOAD";
        case NODE_MEMBER: return "MEMBER";
        case NODE_CALL: return "CALL";
        case NODE_PRE_INCREMENT: return "PRE_INCREMENT";
        case NODE_PRE_DECREMENT: return "PRE_DECREMENT";
        case NODE_POST_INCREMENT: return "POST_INCREMENT";
        case NODE_POST_DECREMENT: return "POST_DECREMENT";
        case NODE_ASSIGNMENT: return "ASSIGNMENT";
        case NODE_STATEMENTS: return "STATEMENTS";
        case NODE_IF: return "IF";
        case NODE_LOOP: return "LOOP";
        case NODE_WHILE: return "WHILE";
        case NODE_DO_WHILE: return "DO_WHILE";
        case NODE_MATCH: return "MATCH";
        case NODE_MATCH_ARM: return "MATCH_ARM";
        case NODE_MATCH_ARM_DEFAULT: return "MATCH_ARM_DEFAULT";
        case NODE_RETURN: return "RETURN";
        case NODE_IMPLICIT_RETURN: return "IMPLICIT_RETURN";
        case NODE_BREAK: return "BREAK";
        case NODE_CONTINUE: return "CONTINUE";
        case NODE_INTEGER: return "INTEGER";
        case NODE_BOOLEAN: return "BOOLEAN";
        case NODE_NULL: return "NULL";
        default: return format("UNKNOWN [%d]", type);
    }
}

static void printNode(Node* node);

static void printNodeChildren(Node* node)
{
    switch (node->type) {
        case NODE_VAR:
            printf("%svalue = ", spaces);
            printNode(node->children.varValue);
            return;
        case NODE_FUNCTION:
            printf("%sbody = ", spaces);
            printNode(node->children.functionBody);
            return;
        case NODE_MODULE:
        case NODE_STATEMENTS: {
            printf("%snodes = [\n", spaces);
            indent();

            VEC_EACH(node->children.nodes) {
                printf("%s", spaces);
                Node* declaration = VEC_EACH_GET(node->children.nodes);
                printNode(declaration);
            }

            outdent();
            printf("%s]\n", spaces);
            return;
        }
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
            printf("%sleft = ", spaces);
            printNode(node->children.left);
            printf("%sright = ", spaces);
            printNode(node->children.right);
            return;
        case NODE_NEGATE:
        case NODE_NOT:
        case NODE_LOOP:
        case NODE_MATCH_ARM_DEFAULT:
        case NODE_RETURN:
        case NODE_IMPLICIT_RETURN:
        case NODE_PRE_INCREMENT:
        case NODE_PRE_DECREMENT:
        case NODE_POST_INCREMENT:
        case NODE_POST_DECREMENT:
            printf("%snode = ", spaces);
            printNode(node->children.node);
            return;
        case NODE_MEMBER:
            printf("%svalue = ", spaces);
            printNode(node->children.memberValue);
            printf("%sname = %s\n", spaces, node->children.memberName);
            return;
        case NODE_WHILE:
        case NODE_DO_WHILE:
            printf("%scondition = ", spaces);
            printNode(node->children.whileCondition);
            printf("%sbody = ", spaces);
            printNode(node->children.whileBody);
            return;
        case NODE_IF:
            printf("%scondition = ", spaces);
            printNode(node->children.ifCondition);
            printf("%sbody = ", spaces);
            printNode(node->children.ifBody);

            if (node->children.elseBody) {
                printf("%selse = ", spaces);
                printNode(node->children.elseBody);
            }

            return;
        case NODE_INTEGER:
            printf("%sinteger = %d\n", spaces, node->children.integer);
            return;
        case NODE_BOOLEAN:
            printf("%sinteger = %s\n", spaces, node->children.boolean ? "true" : "false");
            return;
        case NODE_MATCH: {
            printf("%scondition = ", spaces);
            printNode(node->children.matchValue);
            printf("%sarms = [\n", spaces);
            indent();

            VEC_EACH(node->children.matchArms) {
                printf("%s", spaces);
                Node* declaration = VEC_EACH_GET(node->children.matchArms);
                printNode(declaration);
            }

            outdent();
            printf("%s]\n", spaces);
            return;
        }
        case NODE_MATCH_ARM:
            printf("%sexpression = ", spaces);
            printNode(node->children.matchArmExpression);
            printf("%sbody = ", spaces);
            printNode(node->children.matchArmBody);
            return;
    }
}

static void printNode(Node* node)
{
    printf("%s (\n", getNodeName(node->type));
    indent();
    printNodeChildren(node);
    outdent();
    printf("%s)\n", spaces);
}

void debugNode(Node* node)
{
    printf("--- NODES ---\n");
    spacesNumber = 0;
    spaces = "";
    printNode(node);
    printf("\n\n");
}
