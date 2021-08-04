#include "module.h"
#include <stdio.h>
#include "error.h"
#include "util.h"
#include <stdlib.h>

static char* readFile(char* filename)
{
    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        addError("Failed to open \"%s\".", filename);
        throwErrors();
    }
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* buffer = safeMalloc((size + 1) * sizeof(char));
    size_t sizeRead = fread(buffer, sizeof(char), size, file);

    if (sizeRead < size) {
        addError("Failed to read \"%s\".", filename);
        throwErrors();
    }
    
    if (fclose(file)) {
        addError("Failed to close \"%s\".", filename);
        throwErrors();
    }

    buffer[sizeRead] = '\0';

    return buffer;
}

Module* newModuleFromFilename(char* filename)
{
    Module* module = safeMalloc(sizeof(Module));
    module->filename = filename;
    module->name = filename;
    module->source = readFile(filename);

    return module;
}

void freeModule(Module* module)
{
    free(module);
}
