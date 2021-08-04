#ifndef OPAL_MODULE_H
#define OPAL_MODULE_H

typedef struct {
    char* name;
    char* filename;
    char* source;
} Module;

Module* newModuleFromFilename(char* filaname);
void freeModule(Module* module);

#endif
