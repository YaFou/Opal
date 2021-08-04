#include <stdio.h>
#include "module.h"
#include "vector.h"
#include "parse.h"
#include "scan.h"
#include "error.h"
#include "debug.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("[USAGE] opal <filename>\n");

        return 0;
    }

    Module* module = newModuleFromFilename(argv[1]);
    Vector* tokens = scan(module);

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

    optimizeNode(node);
    printf("%d", interpretNode(node));

    freeNode(node);
    freeModule(module);

    return 0;
}
