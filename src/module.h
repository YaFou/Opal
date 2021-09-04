#ifndef OPAL_MODULE_H
#define OPAL_MODULE_H

typedef struct {
    char* projectPath;
    char* name;
    char* source;
} Module;

Module* createModuleFromPath(char* path);
void freeModule(Module* module);

#endif
