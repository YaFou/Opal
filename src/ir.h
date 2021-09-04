#ifndef OPAL_IR_H
#define OPAL_IR_H

#include "parse.h"

typedef enum {
    IR_MOVE,
    IR_ADD,
    IR_SUBSTRACT,
    IR_MULTIPLY,
    IR_DIVIDE,
    IR_MODULO,
    IR_RETURN,
    IR_NEGATE,
    IR_NOT,
    IR_AND,
    IR_OR,
    IR_ALLOCATE,
    IR_STORE,
    IR_LOAD,
    IR_JUMP,
    IR_JUMP_IF_TRUE,
    IR_EQUAL,
    IR_NOT_EQUAL,
    IR_LESS,
    IR_LESS_EQUAL,
    IR_GREATER,
    IR_GREATER_EQUAL,
} InstructionType;

typedef enum {
    OPERAND_INTEGER,
    OPERAND_REGISTER,
    OPERAND_LABEL,
} OperandType;

typedef struct {
    int virtualNumber;
    int realNumber;
} Register;

typedef struct {
    OperandType type;
    union {
        int integer;
        Register* reg;
    } value;
} Operand;

typedef struct {
    InstructionType type;
    Operand* operand1;
    Operand* operand2;
    Operand* operand3;
} Instruction;

typedef struct {
    int number;
    Vector* instructions;
} Label;

typedef struct {
    char* name;
    int argumentCount;
    Vector* labels;
    int nextLabelNumber;
    int nextRegisterNumber;
} Function;

typedef struct {
    Vector* functions;
} IR;

IR* generateIR(Node* node);
void freeIR(IR* ir);

#endif
