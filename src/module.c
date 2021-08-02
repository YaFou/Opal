#include "module.h"
#include <stdlib.h>
#include "error.h"

Module* newModuleFromFile(char* filename, FILE* file)
{
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char* buffer = malloc(fileSize + 1);

    if (buffer == NULL) {
        throwFailedAlloc();
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

    if (bytesRead < fileSize) {
        addError("Failed to read \"%s\".", filename);
        throwErrors();
    }

    buffer[bytesRead] = '\0';
    Module* module = malloc(sizeof(module));

    if (module == NULL) {
        throwFailedAlloc();
    }

    module->name = filename;
    module->code = buffer;

    return module;
}

void freeModule(Module* module)
{
    if (module->tokens != NULL) {
        freeVector(module->tokens);
    }

    free(module);
}
