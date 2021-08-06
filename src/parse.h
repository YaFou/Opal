#ifndef OPAL_PARSE_H
#define OPAL_PARSE_H

#include "module.h"
#include "vector.h"

typedef enum {
    // INSTRUCTIONS
    NODE_ADD,
    NODE_SUBSTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULO,
    NODE_NEGATE,
    NODE_POWER,

    // VALUES
    NODE_INTEGER
} NodeType;

typedef struct Node {
    NodeType type;

    union {
        int integer;
        struct Node* node;

        struct {
            struct Node* left;
            struct Node* right;
        };
    } children;

    int startIndex;
    int endIndex;
} Node;

Node* parse(Module* module, Vector* tokens);
void freeNode(Node* node);
void optimizeNode(Module* module, Node* node);

#endif
