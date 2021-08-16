#ifndef OPAL_PARSE_H
#define OPAL_PARSE_H

#include "module.h"
#include "vector.h"
#include <stdbool.h>
#include "symbol.h"

typedef enum {
    // STRUCTS
    NODE_STATEMENTS,
    NODE_ASSIGNMENT,
    NODE_LOAD,

    // INSTRUCTIONS
    NODE_ADD,
    NODE_SUBSTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULO,
    NODE_NEGATE,
    NODE_POWER,

    // VALUES
    NODE_INTEGER,
    NODE_BOOLEAN,
    NODE_NULL,
} NodeType;

typedef struct Node {
    NodeType type;

    union {
        int integer;
        struct Node* node;
        bool boolean;
        Vector* nodes;
        Variable* variable;

        struct {
            struct Node* left;
            struct Node* right;
        };

        struct {
            Variable* variableAssignment;
            struct Node* variableValue;
        };
    } children;

    int startIndex;
    int endIndex;
    char* valueType;
} Node;

Node* parse(Module* module, Vector* tokens);
void freeNode(Node* node);
void optimizeNode(Module* module, Node* node);

#endif
