#ifndef OPAL_SYMBOL_H
#define OPAL_SYMBOL_H

#include "map.h"

typedef struct Environment {
    struct Environment* parent;
    Map* variables;
} Environment;

typedef struct {
    char* type;
    int offset;
} Variable;

Environment* newEnvironment();
Variable* newEnvironmentVariable(Environment* environment, char* name, char* type);
void freeEnvironment(Environment* environment);
Variable* getEnvironmentVariable(Environment* environment, char* name);

#endif
