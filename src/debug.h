#ifndef OPAL_DEBUG_C
#define OPAL_DEBUG_C

#include "vector.h"
#include "parse.h"

#define DEBUG

void debugTokens(Vector* tokens);
int interpretNode(Node* node);

#endif
