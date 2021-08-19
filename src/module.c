#include "module.h"
#include <stdio.h>
#include "util.h"
#include "memory.h"
#include <stdlib.h>

Module* createModuleFromPath(const char* path)
{
    Module* module = safeMalloc(sizeof(Module));
    module->projectPath = path;
    module->name = path;
    module->source = readFile(path);

    return module;
}

void freeModule(Module* module)
{
    free(module->source);
    free(module);
}
