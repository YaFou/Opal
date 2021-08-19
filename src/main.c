#include "module.h"
#include <stdio.h>
#include "util.h"
#include "scan.h"
#include "debug.h"
#include "error.h"

int main(int argc, const char** argv)
{
    if (argc == 1) {
        printf("[USAGE] opal <module>\n");

        return 0;
    }

    Module* module = createModuleFromPath(argv[1]);

    printf("Scanning module \"%s\".\n", module->name);
    Vector* tokens = scan(module);
    throwErrors();
    // debugTokens(tokens);

    freeTokens(tokens);
    freeModule(module);

    return 0;
}
