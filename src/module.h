#ifndef OPAL_MODULE_H
#define OPAL_MODULE_H

#include <stdio.h>
#include "vector.h"

typedef struct {
    char* name;
    char* code;
    Vector* tokens;
} Module;

Module* newModuleFromFile(char* filaname, FILE* file);
void freeModule(Module* module);

#endif
