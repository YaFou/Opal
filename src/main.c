#include <stdio.h>
#include "module.h"
#include "vector.h"
#include "parse.h"
#include "scan.h"
#include "error.h"
#include "debug.h"
#include "ir.h"
#include "arch.h"
#include <stdlib.h>

static void throwErrorsIfNeeded()
{
    if (hasErrors()) {
        fprintf(stderr, "Compilation failed.\n");
        throwErrors();
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("[USAGE] opal <filename>\n");

        return 0;
    }

    Module* module = newModuleFromFilename(argv[1]);

    // SCANNING
    printf("Scanning module \"%s\"...\n", module->name);
    Vector* tokens = scan(module);
    throwErrorsIfNeeded();
    // debugTokens(tokens);

    // PARSING
    printf("Parsing module \"%s\"...\n", module->name);
    Node* node = parse(module, tokens);
    freeVector(tokens);
    throwErrorsIfNeeded();
    optimizeNode(module, node);
    throwErrorsIfNeeded();
    // printf("%d", interpretNode(node));

    // GENERATING IR
    IR* ir = generateIR(node);
    freeNode(node);
    // printf("%s", dumpIR(ir));
    // interpretIR(ir);

    // GENERATING ASSEMBLY
    char* assemblyCode = generateAssembly(ir);
    freeIR(ir);
    // printf("%s", assemblyCode);

    freeModule(module);

    FILE* generated = fopen("generated.s", "w");
    fputs(assemblyCode, generated);
    fclose(generated);
    system("gcc generated.s -o program");

    return 0;
}
