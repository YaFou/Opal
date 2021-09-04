#ifndef OPAL_DEBUG_H
#define OPAL_DEBUG_H

#include "util.h"
#include "parse.h"
#include "ir.h"

void debugTokens(Vector* tokens);
void debugNode(Node* node);
void debugIR(IR* ir);

#endif
