#include "ir.h"
#include <stdlib.h>
#include "stringbuilder.h"
#include "util.h"

static IR* makeIR()
{
    IR* ir = safeMalloc(sizeof(IR));
    ir->procedures = newVector();

    return ir;
}

static Procedure* makeProcedure(IR* ir, char* name)
{
    Procedure* procedure = safeMalloc(sizeof(Procedure));
    procedure->name = name;
    procedure->instructions = newVector();
    procedure->nextRegisterNumber = 0;
    pushVector(ir->procedures, procedure);

    return procedure;
}

static Instruction* makeInstruction(Procedure* procedure, InstructionType type)
{
    Instruction* instruction = safeMalloc(sizeof(Instruction));
    instruction->type = type;
    instruction->operands = newVector();
    pushVector(procedure->instructions, instruction);

    return instruction;
}

static void makeInstruction0(Procedure* procedure, InstructionType type)
{
    makeInstruction(procedure, type);
}

static void makeInstruction1(Procedure* procedure, InstructionType type, Operand* operand)
{
    Instruction* instruction = makeInstruction(procedure, type);
    pushVector(instruction->operands, operand);
}

static void makeInstruction2(Procedure* procedure, InstructionType type, Operand* operand1, Operand* operand2)
{
    Instruction* instruction = makeInstruction(procedure, type);
    pushVector(instruction->operands, operand1);
    pushVector(instruction->operands, operand2);
}

static void makeInstruction3(Procedure* procedure, InstructionType type, Operand* operand1, Operand* operand2, Operand* operand3)
{
    Instruction* instruction = makeInstruction(procedure, type);
    pushVector(instruction->operands, operand1);
    pushVector(instruction->operands, operand2);
    pushVector(instruction->operands, operand3);
}

static Operand* makeOperandFromInteger(int integer)
{
    Operand* operand = safeMalloc(sizeof(Operand));
    operand->type = OPERAND_INTEGER;
    operand->value.integer = integer;

    return operand;
}

static Operand* makeOperandFromRegister(Register* reg)
{
    Operand* operand = safeMalloc(sizeof(Operand));
    operand->type = OPERAND_REGISTER;
    operand->value.reg = reg;

    return operand;
}

static Operand* makeRegister(Procedure* procedure)
{
    Register* reg = safeMalloc(sizeof(Register));
    reg->virtualNumber = procedure->nextRegisterNumber++;
    reg->realNumber = -1;

    return makeOperandFromRegister(reg);
}

static Operand* generateBinaryOperation(Procedure* procedure, Node* node)
{
    Operand* returnReg = makeRegister(procedure);

    if (node->type == NODE_INTEGER) {
        makeInstruction2(procedure, IR_MOVE, makeOperandFromInteger(node->children.integer), returnReg);

        return returnReg;
    }

    Node* leftNode = node->children.left;
    Node* rightNode = node->children.right;
    Operand* leftReg = leftNode->type == NODE_INTEGER ? makeOperandFromInteger(leftNode->children.integer) : generateBinaryOperation(procedure, leftNode);
    Operand* rightReg = rightNode->type == NODE_INTEGER ? makeOperandFromInteger(rightNode->children.integer) : generateBinaryOperation(procedure, rightNode);
    InstructionType type;

    switch (node->type) {
        case NODE_ADD:
            type = IR_ADD;
            break;
        case NODE_SUBSTRACT:
            type = IR_SUBSTRACT;
            break;
        case NODE_MULTIPLY:
            type = IR_MULTIPLY;
            break;
        case NODE_DIVIDE:
            type = IR_DIVIDE;
            break;
        case NODE_MODULO:
            type = IR_MODULO;
            break;
    }

    makeInstruction3(procedure, type, leftReg, rightReg, returnReg);

    return returnReg;
}

IR* generateIR(Node* node)
{
    IR* ir = makeIR();
    Procedure* procedure = makeProcedure(ir, "main");

    if (node->type == NODE_INTEGER) {
        makeInstruction1(procedure, IR_RETURN, makeOperandFromInteger(node->children.integer));

        return ir;
    }

    Operand* reg = generateBinaryOperation(procedure, node);
    makeInstruction1(procedure, IR_RETURN, reg);

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
    }
}

static void dumpInstruction(StringBuilder* builder, Instruction* instruction)
{
    appendStringBuilder(builder, format("    %s ", dumpInstructionType(instruction->type)));
    
    for (VECTOR_EACH(instruction->operands)) {
        Operand* operand = VECTOR_GET(instruction->operands, i);

        switch (operand->type) {
            case OPERAND_INTEGER:
                appendStringBuilder(builder, format("%d", operand->value.integer));
                break;
            case OPERAND_REGISTER:
                appendStringBuilder(builder, format("%%%d", operand->value.reg->virtualNumber));
                break;
        }

        if (i != VECTOR_SIZE(instruction->operands) - 1) {
            appendStringBuilder(builder, ", ");
        }
    }

    addStringBuilder(builder, '\n');
}

static void dumpProcedure(StringBuilder* builder, Procedure* procedure)
{
    appendStringBuilder(builder, format("%s\n", procedure->name));

    for (VECTOR_EACH(procedure->instructions)) {
        dumpInstruction(builder, VECTOR_GET(procedure->instructions, i));
    }
}

char* dumpIR(IR* ir)
{
    StringBuilder* builder = newStringBuilder();

    for (VECTOR_EACH(ir->procedures)) {
        dumpProcedure(builder, VECTOR_GET(ir->procedures, i));
    }

    char* dumpedIR = buildStringBuilder(builder);
    freeStringBuilder(builder);

    return dumpedIR;
}
