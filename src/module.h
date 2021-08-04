#ifndef OPAL_MODULE_H
#define OPAL_MODULE_H

#include "vector.h"

typedef struct {
    char* name;
    char* filename;
    char* source;
    Vector* tokens;
} Module;

Module* newModuleFromFilename(char* filaname);
void freeModule(Module* module);

#endif
