#include "arch.h"
#include "stringbuilder.h"
#include "util.h"
#include "map.h"
#include <string.h>

#define REGISTERS_COUNT 4
#define OPERAND(instruction, index) VECTOR_GET(instruction->operands, index)

char* registers[REGISTERS_COUNT] = {"%eax", "%ebx", "%ecx", "%edx"};

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
    switch (operand->type) {
        case OPERAND_INTEGER:
            return format("$%d", operand->value.integer);
        case OPERAND_REGISTER: {
            Register* reg = operand->value.reg;

            if (reg->realNumber == -1) {
                reg->realNumber = getFreeRegister();
            }

            return registers[reg->realNumber];
        }
        case OPERAND_MEMORY:
            return format("%d(%%esp)", -operand->value.integer - 4);
    }
}

static void freeRegister(int reg)
{
    generator->usedRegisters[reg] = false;
}

static void freeOperand(Operand* operand)
{
    switch (operand->type) {
        case OPERAND_REGISTER:
            freeRegister(operand->value.reg->realNumber);
            break;
    }
}

static void binaryOperation(Instruction* instruction, char* operation)
{
    Operand* operand1 = OPERAND(instruction, 0);
    Operand* operand2 = OPERAND(instruction, 1);

    char* value1 = operand(operand1);
    char* value2 = operand(operand2);
    char* resultReg = operand(OPERAND(instruction, 2));

    emitLine(format("movl %s, %s", value2, resultReg));
    emitLine(format("%s %s, %s", operation, value1, resultReg));

    freeOperand(operand1);
    freeOperand(operand2);
}

static void divide(Instruction* instruction)
{
    Operand* operand1 = OPERAND(instruction, 0);
    Operand* operand2 = OPERAND(instruction, 1);

    char* value1 = operand(operand1);
    char* value2 = operand(operand2);
    char* resultReg = operand(OPERAND(instruction, 2));

    emitLine(format("movl %s, %%eax", value1));
    emitLine(format("movl %s, %%ebx", value2));
    emitLine(format("idivl %%ebx", value2));
    emitLine(format("movl %%eax, %s", resultReg));

    freeOperand(operand1);
    freeOperand(operand2);
}

static void move(Instruction* instruction)
{
    Operand* source = OPERAND(instruction, 0);
    char* destination = operand(OPERAND(instruction, 1));
    emitLine(format("movl %s, %s", operand(source), destination));
    freeOperand(source);
}

static void ret(Instruction* instruction)
{
    char* source = operand(OPERAND(instruction, 0));
    emitLine(format("movl %s, %%eax", source));
    emitLine("movl $D0, (%esp)");
    emitLine("movl %eax, 4(%esp)");
    emitLine("call _printf");
    emitLine("leave");
    emitLine("ret");
}

static void instruction(Instruction* instruction)
{
    switch (instruction->type) {
        case IR_ADD:
            binaryOperation(instruction, "addl");
            break;
        case IR_SUBSTRACT:
            binaryOperation(instruction, "subl");
            break;
        case IR_MULTIPLY:
            binaryOperation(instruction, "imull");
            break;
        case IR_DIVIDE:
            divide(instruction);
            break;
        case IR_MODULO:
            break;
        case IR_MOVE:
            move(instruction);
            break;
        case IR_RETURN:
            ret(instruction);
            break; 
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
    emit(
        "    .globl _main\n"
        "D0: .ascii \"%d\\0\"\n"
        "_main:\n"
        "    pushl %ebp\n"
        "    movl %esp, %ebp\n"
        "    subl $32, %esp\n"
    );

    for (VECTOR_EACH(ir->procedures)) {
        procedure(VECTOR_GET(ir->procedures, i));
    }

    char* code = buildStringBuilder(generator->builder);
    freeGenerator(generator);

    return code;
}
