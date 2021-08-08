#ifndef OPAL_DEBUG_C
#define OPAL_DEBUG_C

#include "vector.h"
#include "parse.h"
#include "ir.h"

void debugTokens(Vector* tokens);
int interpretNode(Node* node);
void interpretIR(IR* ir);

#endif
