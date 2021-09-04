#include "module.h"
#include <stdio.h>
#include "util.h"
#include "scan.h"
#include "error.h"
#include "parse.h"
#include "types.h"
#include "ir.h"

#ifdef _DEBUG
#include "debug.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

static void throwErrorsIfNeeded()
{
    if (hasErrors()) {
        fprintf(stderr, tty() ? CURSOR_UP_ERASE TEXT_RED "=== Compilation failed.\n" : "=== Compilation failed.\n");
        throwErrors();
    }
}

int main(int argc, char** argv)
{
    #ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(handle, &mode);
    mode |= 4;
    SetConsoleMode(handle, mode);
    #endif

    if (argc == 1) {
        printf(tty() ?
            TEXT_BOLD "[USAGE]" TEXT_RESET " opal <module>\n" :
            "[USAGE] opal <module>\n"
        );

        return 0;
    }

    Module* module = createModuleFromPath(argv[1]);

    printf("[1/4] Scanning module \"%s\".\n", module->name);
    Vector* tokens = scan(module);
    throwErrorsIfNeeded();

    #ifdef _DEBUG
    debugTokens(tokens);
    #endif

    printf(tty() ?
        CURSOR_UP_ERASE "[2/4] Parsing module \"%s\".\n" :
        "[2/4] Parsing module \"%s\".\n",
        module->name
    );
    Node* node = parse(module, tokens);
    throwErrorsIfNeeded();
    freeTokens(tokens);

    printf(tty() ?
        CURSOR_UP_ERASE "[3/4] Checking types of module \"%s\".\n" :
        "[3/4] Checking types of module \"%s\".\n",
        module->name
    );
    checkTypes(module, node);
    throwErrorsIfNeeded();

    #ifdef _DEBUG
    debugNode(node);
    #endif

    printf(tty() ?
        CURSOR_UP_ERASE "[4/4] Generating IR of module \"%s\".\n" :
        "[4/4] Generating IR of module \"%s\".\n",
        module->name
    );
    IR* ir = generateIR(node);
    freeNode(node);

    #ifdef _DEBUG
    debugIR(ir);
    #endif

    printf(tty() ?
        CURSOR_UP_ERASE TEXT_GREEN "=== Compilation has succeed.\n" TEXT_RESET :
        "=== Compilation has succeed.\n",
        module->name
    );
    throwWarnings();
    freeIR(ir);
    freeModule(module);

    return 0;
}
