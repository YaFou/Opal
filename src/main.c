#include "module.h"
#include <stdio.h>
#include "util.h"
#include "scan.h"
#include "error.h"
#include "parse.h"
#include "types.h"

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

int main(int argc, const char** argv)
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

    printf("[1/3] Scanning module \"%s\".\n", module->name);
    Vector* tokens = scan(module);
    throwErrorsIfNeeded();

    #ifdef _DEBUG
    debugTokens(tokens);
    #endif

    printf(tty() ?
        CURSOR_UP_ERASE "[2/3] Parsing module \"%s\".\n" :
        "[2/3] Parsing module \"%s\".\n",
        module->name
    );
    Node* node = parse(module, tokens);
    throwErrorsIfNeeded();
    freeTokens(tokens);

    printf(tty() ?
        CURSOR_UP_ERASE "[3/3] Checking types of module \"%s\".\n" :
        "[3/3] Checking types of module \"%s\".\n",
        module->name
    );
    checkTypes(module, node);
    throwErrorsIfNeeded();

    #ifdef _DEBUG
    debugNode(node);
    #endif

    printf(tty() ?
        CURSOR_UP_ERASE TEXT_GREEN "=== Compilation has succeed.\n" TEXT_RESET :
        "=== Compilation has succeed.\n",
        module->name
    );
    throwWarnings();
    freeNode(node);
    freeModule(module);

    return 0;
}
