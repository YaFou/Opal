#ifndef OPAL_MODULE_H
#define OPAL_MODULE_H

typedef struct {
    const char* projectPath;
    const char* name;
    char* source;
} Module;

Module* createModuleFromPath(const char* path);
void freeModule(Module* module);

#endif
