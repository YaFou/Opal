#ifndef OPAL_PARSE_H
#define OPAL_PARSE_H

#include "module.h"
#include "util.h"
#include <stdbool.h>

typedef enum {
    // STRUCTURES
    NODE_FUNCTION,
    NODE_MODULE,
    NODE_VAR,

    // OPERATIONS
    NODE_ADD,
    NODE_SUBSTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULO,
    NODE_POWER,
    NODE_AND,
    NODE_OR,
    NODE_NEGATE,
    NODE_NOT,
    NODE_EQUAL,
    NODE_NOT_EQUAL,
    NODE_LESS,
    NODE_LESS_EQUAL,
    NODE_GREATER,
    NODE_GREATER_EQUAL,
    NODE_ASSIGNMENT,
    NODE_ADD_ASSIGNMENT,
    NODE_SUBSTRACT_ASSIGNMENT,
    NODE_MULTIPLY_ASSIGNMENT,
    NODE_DIVIDE_ASSIGNMENT,
    NODE_MODULO_ASSIGNMENT,
    NODE_POWER_ASSIGNMENT,

    // CONTROL FLOW
    NODE_STATEMENTS,
    NODE_IF,
    NODE_LOOP,
    NODE_WHILE,
    NODE_MATCH,
    NODE_MATCH_ARM,
    NODE_RETURN,

    // LITERALS
    NODE_INTEGER,
    NODE_BOOLEAN,
} NodeType;

typedef struct Node {
    // HEADER
    Module* module;
    int startIndex;
    int endIndex;

    // BODY
    NodeType type;
    union {
        int integer;
        bool boolean;
        struct Node* node;
        Vector* nodes;

        struct {
            struct Node* left;
            struct Node* right;
        };

        struct {
            struct Node* ifCondition;
            struct Node* ifBody;
            struct Node* elseBody;
        };

        struct {
            struct Node* whileCondition;
            struct Node* whileBody;
        };

        struct {
            struct Node* varValue;
        };

        struct {
            struct Node* matchValue;
            Vector* matchArms;
        };
        
        struct  {
            struct Node* matchArmExpression;
            struct Node* matchArmBody;
        };

        struct {
            Vector* functionParameters;
            struct Node* functionBody;
        };
    } children;
} Node;

Node* parse(Module* module, Vector* tokens);
void freeNode(Node* node);

#endif
