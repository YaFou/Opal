#include "ir.h"
#include <stdlib.h>
#include "stringbuilder.h"
#include "util.h"
#include "error.h"

IR* ir;
Procedure* procedure;

static IR* makeIR()
{
    IR* ir = safeMalloc(sizeof(IR));
    ir->procedures = newVector();

    return ir;
}

static Procedure* makeProcedure(char* name)
{
    Procedure* procedure = safeMalloc(sizeof(Procedure));
    procedure->name = name;
    procedure->instructions = newVector();
    procedure->nextRegisterNumber = 0;
    procedure->nextSubProcedureNumber = 0;
    pushVector(ir->procedures, procedure);

    return procedure;
}

static Procedure* makeSubProcedure(Procedure* procedure)
{
    return makeProcedure(format("%s:%d", procedure->name, procedure->nextSubProcedureNumber++));
}

static Instruction* makeInstruction(InstructionType type)
{
    Instruction* instruction = safeMalloc(sizeof(Instruction));
    instruction->type = type;
    instruction->operands = newVector();
    pushVector(procedure->instructions, instruction);

    return instruction;
}

static void makeInstruction0(InstructionType type)
{
    makeInstruction(type);
}

static void makeInstruction1(InstructionType type, Operand* operand)
{
    Instruction* instruction = makeInstruction(type);
    pushVector(instruction->operands, operand);
}

static void makeInstruction2(InstructionType type, Operand* operand1, Operand* operand2)
{
    Instruction* instruction = makeInstruction(type);
    pushVector(instruction->operands, operand1);
    pushVector(instruction->operands, operand2);
}

static void makeInstruction3(InstructionType type, Operand* operand1, Operand* operand2, Operand* operand3)
{
    Instruction* instruction = makeInstruction(type);
    pushVector(instruction->operands, operand1);
    pushVector(instruction->operands, operand2);
    pushVector(instruction->operands, operand3);
}

static Operand* makeOperand(OperandType type)
{
    Operand* operand = safeMalloc(sizeof(Operand));
    operand->type = type;
    
    return operand;
}

static Operand* makeOperandFromInteger(int integer)
{
    Operand* operand = makeOperand(OPERAND_INTEGER);
    operand->value.integer = integer;

    return operand;
}

static Operand* makeOperandFromRegister(Register* reg)
{
    Operand* operand = makeOperand(OPERAND_REGISTER);
    operand->value.reg = reg;

    return operand;
}

static Operand* makeRegister()
{
    Register* reg = safeMalloc(sizeof(Register));
    reg->virtualNumber = procedure->nextRegisterNumber++;
    reg->realNumber = -1;

    return makeOperandFromRegister(reg);
}

static Operand* generateNode(Node* node);

static Operand* binaryOperation(Node* node, InstructionType type)
{
    Operand* value1 = generateNode(node->children.left);
    Operand* value2 = generateNode(node->children.right);
    Operand* result = makeRegister(procedure);
    makeInstruction3(type, value1, value2, result);

    return result;
}

static Operand* generateNode(Node* node)
{
    switch (node->type) {
        case NODE_ADD:
            return binaryOperation(node, IR_ADD);
        case NODE_SUBSTRACT:
            return binaryOperation(node, IR_SUBSTRACT);
        case NODE_MULTIPLY:
            return binaryOperation(node, IR_MULTIPLY);
        case NODE_DIVIDE:
            return binaryOperation(node, IR_DIVIDE);
        case NODE_MODULO:
            return binaryOperation(node, IR_MODULO);
        case NODE_NEGATE: {
            Operand* value = generateNode(node->children.node);
            Operand* result = makeRegister(procedure);
            makeInstruction2(IR_NEGATE, value, result);

            return result;
        }
        case NODE_INTEGER: {
            Operand* value = makeOperandFromInteger(node->children.integer);
            Operand* result = makeRegister(procedure);
            makeInstruction2(IR_MOVE, value, result);

            return result;
        }
        case NODE_POWER:
            throwFatal("Raised a value to a power is not supported yet.");
        case NODE_BOOLEAN: {
            Operand* value = makeOperandFromInteger(node->children.boolean);
            Operand* result = makeRegister(procedure);
            makeInstruction2(IR_MOVE, value, result);

            return result;
        }
    }
}

IR* generateIR(Node* node)
{
    ir = makeIR();
    procedure = makeProcedure("main");
    Operand* reg = generateNode(node);
    makeInstruction1(IR_RETURN, reg);

    return ir;
}

static void freeInstruction(Instruction* instruction)
{
    for (VECTOR_EACH(instruction->operands)) {
        free(VECTOR_GET(instruction->operands, i));
    }

    freeVector(instruction->operands);
    free(instruction);
}

static void freeProcedure(Procedure* procedure)
{
    for (VECTOR_EACH(procedure->instructions)) {
        freeInstruction((Instruction*) VECTOR_GET(procedure->instructions, i));
    }

    freeVector(procedure->instructions);
    free(procedure);
}

void freeIR(IR* ir)
{
    for (VECTOR_EACH(ir->procedures)) {
        freeProcedure((Procedure*) VECTOR_GET(ir->procedures, i));
    }

    freeVector(ir->procedures);
    free(ir);
}

StringBuilder* builder;

static char* dumpInstructionType(InstructionType type)
{
    switch (type) {
        case IR_ADD:
            return "ADD";
        case IR_SUBSTRACT:
            return "SUB";
        case IR_MULTIPLY:
            return "MUL";
        case IR_DIVIDE:
            return "DIV";
        case IR_MODULO:
            return "MOD";
        case IR_RETURN:
            return "RET";
        case IR_MOVE:
            return "MOV";
        case IR_NEGATE:
            return "NEG";
    }
}

static void emit(char* code)
{
    appendStringBuilder(builder, code);
}

static void dumpInstruction(Instruction* instruction)
{
    emit(format("    %s ", dumpInstructionType(instruction->type)));
    
    for (VECTOR_EACH(instruction->operands)) {
        Operand* operand = VECTOR_GET(instruction->operands, i);

        switch (operand->type) {
            case OPERAND_INTEGER:
                emit(format("%d", operand->value.integer));
                break;
            case OPERAND_REGISTER:
                emit(format("%%%d", operand->value.reg->virtualNumber));
                break;
        }

        if (i != VECTOR_SIZE(instruction->operands) - 1) {
            emit(", ");
        }
    }

    emit("\n");
}

static void dumpProcedure(Procedure* procedure)
{
    emit(format("%s\n", procedure->name));

    for (VECTOR_EACH(procedure->instructions)) {
        dumpInstruction(VECTOR_GET(procedure->instructions, i));
    }

    emit("\n");
}

char* dumpIR(IR* ir)
{
    builder = newStringBuilder();

    for (VECTOR_EACH(ir->procedures)) {
        dumpProcedure(VECTOR_GET(ir->procedures, i));
    }

    char* dumpedIR = buildStringBuilder(builder);
    freeStringBuilder(builder);

    return dumpedIR;
}
