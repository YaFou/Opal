#include "arch.h"
#include "stringbuilder.h"
#include "util.h"
#include "map.h"
#include <string.h>

#define REGISTERS_COUNT 4

char* registers[REGISTERS_COUNT] = {"%eax", "%ebx", "%cbx", "%dbx"};

typedef struct {
    int nextLabelNumber;
    StringBuilder* builder;
    Map* procedures;
    bool usedRegisters[REGISTERS_COUNT];
} Generator;

static Generator* makeGenerator()
{
    Generator* generator = safeMalloc(sizeof(Generator));
    generator->nextLabelNumber = 0;
    generator->builder = newStringBuilder();
    generator->procedures = newMap();
    
    for (int i = 0; i < REGISTERS_COUNT; i++) {
        generator->usedRegisters[i] = false;
    }

    return generator;
}

static void freeGenerator(Generator* generator)
{
    freeStringBuilder(generator->builder);
    freeMap(generator->procedures);
    free(generator);
}

static char* makeLabel(Generator *generator)
{
    return format("L%d", generator->nextLabelNumber++);
}

static void emit(Generator* generator, char* code)
{
    appendStringBuilder(generator->builder, code);
}

static void emitLine(Generator* generator, char* line)
{
    emit(generator, format("    %s\n", line));
}

static int getFreeRegister(Generator* generator)
{
    for (int i = 0; i < REGISTERS_COUNT; i++) {
        if (generator->usedRegisters[i] == false) {
            generator->usedRegisters[i] = true;

            return i;
        }
    }
}

static char* generateOperand(Generator* generator, Operand* operand)
{
    if (operand->type == OPERAND_INTEGER) {
        return format("$%d", operand->value.integer);
    }

    Register* reg = operand->value.reg;

    if (reg->realNumber == -1) {
        reg->realNumber = getFreeRegister(generator);
    }

    return registers[reg->realNumber];
}

static void freeRegister(Generator* generator, int reg)
{
    generator->usedRegisters[reg] = false;
}

static void freeOperand(Generator* generator, Operand* operand)
{
    if (operand->type == OPERAND_INTEGER) {
        return;
    }

    freeRegister(generator, operand->value.reg->realNumber);
}

static void binaryOperation(Generator* generator, Instruction* instruction, char* operation)
{
    Operand* operand1 = VECTOR_GET(instruction->operands, 0);
    Operand* operand2 = VECTOR_GET(instruction->operands, 1);

    char* value1 = generateOperand(generator, operand1);
    char* value2 = generateOperand(generator, operand2);
    char* resultReg = generateOperand(generator, VECTOR_GET(instruction->operands, 2));

    emitLine(generator, format("mov %s, %s", value2, resultReg));
    emitLine(generator, format("%s %s, %s", operation, value1, resultReg));

    freeOperand(generator, operand1);
    freeOperand(generator, operand2);
}

static void generateInstruction(Generator* generator, Instruction* instruction)
{
    switch (instruction->type) {
        case IR_ADD:
            binaryOperation(generator, instruction, "add");
            break;
        case IR_SUBSTRACT:
            binaryOperation(generator, instruction, "sub");
            break;
        case IR_MULTIPLY:
            binaryOperation(generator, instruction, "imul");
            break;
        case IR_DIVIDE: {
            Operand* operand1 = VECTOR_GET(instruction->operands, 0);
            Operand* operand2 = VECTOR_GET(instruction->operands, 1);

            char* value1 = generateOperand(generator, operand1);
            char* value2 = generateOperand(generator, operand2);
            char* resultReg = generateOperand(generator, VECTOR_GET(instruction->operands, 2));

            emitLine(generator, format("mov %s, %%eax", value1));
            emitLine(generator, format("mov %s, %%ebx", value2));
            // emitLine(generator, format("cqo"));
            emitLine(generator, format("idiv %%ebx", value2));

            if (strcmp(resultReg, "%eax")) {
                emitLine(generator, format("mov %%eax, %s", resultReg));
            }

            freeOperand(generator, operand1);
            freeOperand(generator, operand2);
            break;
        }
        case IR_MODULO:
        case IR_MOVE:
        case IR_RETURN: {
            char* source = generateOperand(generator, VECTOR_GET(instruction->operands, 0));

            if (strcmp(source, "%eax")) {
                emitLine(generator, format("mov %s, %%eax", source));
            }
            
            emitLine(generator, "ret");
            break;
        }
            
    }
}

static void generateProcedure(Generator* generator, Procedure* procedure)
{
    char* label = makeLabel(generator);
    setMap(generator->procedures, procedure->name, label);
    emit(generator, format("%s:\n"));

    for (VECTOR_EACH(procedure->instructions)) {
        generateInstruction(generator, VECTOR_GET(procedure->instructions, i));
    }
}

char* generateAssembly(IR* ir)
{
    Generator* generator = makeGenerator();

    for (VECTOR_EACH(ir->procedures)) {
        generateProcedure(generator, VECTOR_GET(ir->procedures, i));
    }

    char* code = buildStringBuilder(generator->builder);
    freeGenerator(generator);

    return code;
}
