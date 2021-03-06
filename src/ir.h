#ifndef OPAL_IR_H
#define OPAL_IR_H

#include "parse.h"

typedef enum {
    IR_ADD,
    IR_SUBSTRACT,
    IR_MULTIPLY,
    IR_DIVIDE,
    IR_MODULO,
    IR_RETURN,
    IR_MOVE,
    IR_NEGATE,
} InstructionType;

typedef struct {
    int virtualNumber;
    int realNumber;
} Register;

typedef enum {
    OPERAND_INTEGER,
    OPERAND_REGISTER,
    OPERAND_MEMORY,
} OperandType;

typedef struct {
    OperandType type;
    union {
        Register* reg;
        int integer;
    } value;
} Operand;

typedef struct {
    InstructionType type;
    Vector* operands;
} Instruction;

typedef struct {
    char* name;
    Vector* instructions;
    int nextRegisterNumber;
    int nextSubProcedureNumber;
} Procedure;

typedef struct {
    Vector* procedures;
    int offset;
} IR;

IR* generateIR(Node* node);
void freeIR(IR* ir);
char* dumpIR(IR* ir);

#endif
