#include "scan.h"
#include "error.h"
#include <stdio.h>
#include "debug.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("[USAGE] opal <filename>\n");

        return 0;
    }

    Module* module = newModuleFromFilename(argv[1]);
    module->tokens = scan(module);
    // debugTokens(module->tokens);

    if (hasErrors()) {
        fprintf(stderr, "Compilation failed.\n");
        throwErrors();
    }

    freeVector(module->tokens);
    freeModule(module);

    return 0;
}
