#include "scan.h"
#include "error.h"
#include <stdio.h>
#include "debug.h"
#include "parse.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("[USAGE] opal <filename>\n");

        return 0;
    }

    Module* module = newModuleFromFilename(argv[1]);
    Vector* tokens = scan(module);
    // debugTokens(tokens);

    if (hasErrors()) {
        fprintf(stderr, "Compilation failed.\n");
        throwErrors();
    }

    Node* node = parse(module, tokens);
    freeVector(tokens);

    if (hasErrors()) {
        fprintf(stderr, "Compilation failed.\n");
        throwErrors();
    }

    printf("%d", interpretNode(node));

    freeNode(node);
    freeModule(module);

    return 0;
}
