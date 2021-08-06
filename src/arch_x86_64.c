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

Generator* generator;

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

static char* makeLabel()
{
    return format("L%d", generator->nextLabelNumber++);
}

static void emit(char* code)
{
    appendStringBuilder(generator->builder, code);
}

static void emitLine(char* line)
{
    emit(format("    %s\n", line));
}

static int getFreeRegister()
{
    for (int i = 0; i < REGISTERS_COUNT; i++) {
        if (generator->usedRegisters[i] == false) {
            generator->usedRegisters[i] = true;

            return i;
        }
    }
}

static char* operand(Operand* operand)
{
    if (operand->type == OPERAND_INTEGER) {
        return format("$%d", operand->value.integer);
    }

    Register* reg = operand->value.reg;

    if (reg->realNumber == -1) {
        reg->realNumber = getFreeRegister();
    }

    return registers[reg->realNumber];
}

static void freeRegister(int reg)
{
    generator->usedRegisters[reg] = false;
}

static void freeOperand(Operand* operand)
{
    if (operand->type == OPERAND_INTEGER) {
        return;
    }

    freeRegister(operand->value.reg->realNumber);
}

static void binaryOperation(Instruction* instruction, char* operation)
{
    Operand* operand1 = VECTOR_GET(instruction->operands, 0);
    Operand* operand2 = VECTOR_GET(instruction->operands, 1);

    char* value1 = operand(operand1);
    char* value2 = operand(operand2);
    char* resultReg = operand(VECTOR_GET(instruction->operands, 2));

    emitLine(format("mov %s, %s", value2, resultReg));
    emitLine(format("%s %s, %s", operation, value1, resultReg));

    freeOperand(operand1);
    freeOperand(operand2);
}

static void instruction(Instruction* instruction)
{
    switch (instruction->type) {
        case IR_ADD:
            binaryOperation(instruction, "add");
            break;
        case IR_SUBSTRACT:
            binaryOperation(instruction, "sub");
            break;
        case IR_MULTIPLY:
            binaryOperation(instruction, "imul");
            break;
        case IR_DIVIDE: {
            Operand* operand1 = VECTOR_GET(instruction->operands, 0);
            Operand* operand2 = VECTOR_GET(instruction->operands, 1);

            char* value1 = operand(operand1);
            char* value2 = operand(operand2);
            char* resultReg = operand(VECTOR_GET(instruction->operands, 2));

            emitLine(format("mov %s, %%eax", value1));
            emitLine(format("mov %s, %%ebx", value2));
            emitLine(format("idiv %%ebx", value2));

            if (strcmp(resultReg, "%eax")) {
                emitLine(format("mov %%eax, %s", resultReg));
            }

            freeOperand(operand1);
            freeOperand(operand2);
            break;
        }
        case IR_MODULO:
        case IR_MOVE:
        case IR_RETURN: {
            char* source = operand(VECTOR_GET(instruction->operands, 0));

            if (strcmp(source, "%eax")) {
                emitLine(format("mov %s, %%eax", source));
            }
            
            emitLine("ret");
            break;
        }
            
    }
}

static void procedure(Procedure* procedure)
{
    char* label = makeLabel();
    setMap(generator->procedures, procedure->name, label);
    emit(format("%s:\n", label));

    for (VECTOR_EACH(procedure->instructions)) {
        instruction(VECTOR_GET(procedure->instructions, i));
    }
}

char* generateAssembly(IR* ir)
{
    generator = makeGenerator();

    for (VECTOR_EACH(ir->procedures)) {
        procedure(VECTOR_GET(ir->procedures, i));
    }

    char* code = buildStringBuilder(generator->builder);
    freeGenerator(generator);

    return code;
}
