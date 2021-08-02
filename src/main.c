#include "tokenizer.h"
#include "error.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("[USAGE] opal <filename>\n");

        return 0;
    }

    FILE* file = fopen(argv[1], "r");

    if (file == NULL) {
        addError("Failed to open \"%s\".", argv[1]);
        throwErrors();
    }

    Module* module = newModuleFromFile(argv[1], file);
    fclose(file);
    Vector* tokens = tokenize(module);
    module->tokens = tokens;
    freeModule(module);
    freeVector(tokens);

    return 0;
}
