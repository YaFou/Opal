#include "debug.h"
#include "scan.h"
#include <stdio.h>
#include <math.h>
#include "map.h"
#include "util.h"

void debugTokens(Vector* tokens)
{
    for (VECTOR_EACH(tokens)) {
        Token* token = VECTOR_GET(tokens, i);

        switch (token->type) {
            case TOKEN_PLUS:
                printf("PLUS\n");
                break;
            case TOKEN_MINUS:
                printf("MINUS\n");
                break;
            case TOKEN_STAR:
                printf("STAR\n");
                break;
            case TOKEN_SLASH:
                printf("SLASH\n");
                break;
            case TOKEN_LEFT_PAREN:
                printf("LEFT_PAREN\n");
                break;
            case TOKEN_RIGHT_PAREN:
                printf("RIGHT_PAREN\n");
                break;
            case TOKEN_LEFT_BRACE:
                printf("LEFT_BRACE\n");
                break;
            case TOKEN_RIGHT_BRACE:
                printf("RIGHT_BRACE\n");
                break;
            case TOKEN_SEMILICON:
                printf("SEMILICON\n");
                break;
            case TOKEN_INTEGER:
                printf("INTEGER | %d\n", token->value.integer);
                break;
            case TOKEN_EOF:
                printf("EOF\n");
                break;
            default:
                printf("UNKNOWN\n");
        }
    }
}

int interpretNode(Node* node)
{
    int value;

    #define COMBINE(node, operator) interpretNode(node->children.left) operator interpretNode(node->children.right)
    switch (node->type) {
        case NODE_ADD:
            value = COMBINE(node, +);
            break;
        case NODE_SUBSTRACT:
            value = COMBINE(node, -);
            break;
        case NODE_MULTIPLY:
            value = COMBINE(node, *);
            break;
        case NODE_DIVIDE:
            value = COMBINE(node, /);
            break;
        case NODE_MODULO:
            value = COMBINE(node, %);
            break;
        case NODE_POWER:
            value = pow(interpretNode(node->children.left), interpretNode(node->children.right));
            break;
        case NODE_INTEGER:
            value = node->children.integer;
            break;
        case NODE_NEGATE:
            value = -node->children.node->children.integer;
            break;
    }
    #undef COMBINE

    return value;
}

#define REGISTER_NUMBER(reg) format("%d", reg->virtualNumber)

Procedure* procedure;
Map* registers;

static int loadOperand(Instruction* instruction, int index)
{
    Operand* operand = VECTOR_GET(instruction->operands, index);

    switch (operand->type) {
        case OPERAND_INTEGER:
            return operand->value.integer;
        case OPERAND_REGISTER: {
            return *((int*) getMap(registers, REGISTER_NUMBER(operand->value.reg)));
        }
    }
}

static void storeOperand(Instruction* instruction, int index, int value)
{
    Operand* operand = VECTOR_GET(instruction->operands, index);
    int* pointer = safeMalloc(sizeof(int));
    *pointer = value;
    setMap(registers, REGISTER_NUMBER(operand->value.reg), pointer);
}

static void binaryOperation(Instruction* instruction)
{
    switch (instruction->type) {
        
    }
}

static void interpretProcedure()
{
    for (VECTOR_EACH(procedure->instructions)) {
        Instruction* instruction = VECTOR_GET(procedure->instructions, i);
        
        switch (instruction->type) {
            #define STORE(operator) storeOperand(instruction, 2, loadOperand(instruction, 0) operator loadOperand(instruction, 1))
            case IR_ADD:
                STORE(+);
                break;
            case IR_SUBSTRACT:
                STORE(-);
                break;
            case IR_MULTIPLY:
                STORE(*);
                break;
            case IR_DIVIDE:
                STORE(/);
                break;
            case IR_MODULO:
                STORE(%);
                break;
            #undef STORE

            case IR_RETURN: {
                printf("%d", loadOperand(instruction, 0));
                break;
            }
            case IR_MOVE: {
                int value = loadOperand(instruction, 0);
                storeOperand(instruction, 1, value);
                break;
            }
        }
    }
}

void interpretIR(IR* ir)
{
    registers = newMap();
    procedure = VECTOR_GET(ir->procedures, 0);
    interpretProcedure();

    freeMap(registers);
}
