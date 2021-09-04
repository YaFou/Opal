#include "ir.h"
#include "memory.h"

Function* function;
Label* label;

static Operand* generateExpression(Node* node);
static void generateStatements(Node* node);

static IR* makeIR()
{
    IR* ir = safeMalloc(sizeof(IR));
    ir->functions = newVector();

    return ir;
}

static Label* makeLabel()
{
    Label* label = safeMalloc(sizeof(Label));
    label->number = function->nextLabelNumber++;
    label->instructions = newVector();
    vectorPush(function->labels, label);

    return label;
}

static Operand* makeOperand(OperandType type)
{
    Operand* operand = safeMalloc(sizeof(Operand));
    operand->type = type;

    return operand;
}

static Operand* makeRegister()
{
    Register* reg = safeMalloc(sizeof(Register));
    reg->virtualNumber = function->nextRegisterNumber++;
    reg->realNumber = -1;

    Operand* operand = makeOperand(OPERAND_REGISTER);
    operand->value.reg = reg;

    return operand;
}

static void pushInstruction(Instruction* instruction)
{
    vectorPush(label->instructions, instruction);
}

static Instruction* makeInstruction(InstructionType type)
{
    Instruction* instruction = safeMalloc(sizeof(Instruction));
    instruction->type = type;
    instruction->operand1 = instruction->operand2 = instruction->operand3 = NULL;

    return instruction;
}

static void instruction0(InstructionType type)
{
    Instruction* instruction = makeInstruction(type);
    pushInstruction(instruction);
}

static void instruction1(InstructionType type, Operand* operand)
{
    Instruction* instruction = makeInstruction(type);
    instruction->operand1 = operand;
    pushInstruction(instruction);
}

static void instruction2(InstructionType type, Operand* operand1, Operand* operand2)
{
    Instruction* instruction = makeInstruction(type);
    instruction->operand1 = operand1;
    instruction->operand2 = operand2;
    pushInstruction(instruction);
}

static void instruction3(InstructionType type, Operand* operand1, Operand* operand2, Operand* operand3)
{
    Instruction* instruction = makeInstruction(type);
    instruction->operand1 = operand1;
    instruction->operand2 = operand2;
    instruction->operand3 = operand3;
    pushInstruction(instruction);
}

static Operand* doubleOperation(Node* node, InstructionType type)
{
    Operand* value1 = generateExpression(node->children.left);
    Operand* value2 = generateExpression(node->children.right);
    Operand* destination = makeRegister();
    instruction3(type, value1, value2, destination);

    return destination;
}

static Operand* unaryOperation(Node* node, InstructionType type)
{
    Operand* value = generateExpression(node->children.node);
    Operand* destination = makeRegister();
    instruction2(type, value, destination);

    return destination;
}

static Operand* makeIntegerOperand(int integer)
{
    Operand* operand = makeOperand(OPERAND_INTEGER);
    operand->value.integer = integer;

    return operand;
}

static Operand* makeLabelOperand(Label* label)
{
    Operand* operand = makeOperand(OPERAND_LABEL);
    operand->value.integer = label->number;

    return operand;
}

static void generateIfExpression(Node* node)
{
    Label* start = label;
    Operand* condition = generateExpression(node->children.ifCondition);
    Label* ifLabel = makeLabel();
    instruction2(IR_JUMP_IF_TRUE, condition, makeLabelOperand(ifLabel));
    label = ifLabel;
    generateStatements(node->children.ifBody);
    Label* elseLabel;

    if (node->children.elseBody) {
        elseLabel = makeLabel();
        label = start;
        instruction1(IR_JUMP, makeLabelOperand(elseLabel));
        label = elseLabel;

        if (node->children.elseBody->type == NODE_IF) {
            generateIfExpression(node->children.elseBody);
        } else {
            generateStatements(node->children.elseBody);
        }
    }
    
    Label* exit = makeLabel();
    label = ifLabel;
    instruction1(IR_JUMP, makeLabelOperand(exit));

    if (!node->children.elseBody) {
        label = start;
        instruction1(IR_JUMP, makeLabelOperand(exit));
    }

    label = exit;
}

static void generateWhileExpression(Node* node)
{
    Label* body = makeLabel();
    instruction1(IR_JUMP, makeLabelOperand(body));
    label = body;
    Operand* condition = generateExpression(node->children.whileCondition);
    Label* exit = makeLabel();
    instruction2(IR_JUMP_IF_TRUE, condition, makeLabelOperand(exit));
    generateStatements(node->children.whileBody);
    instruction1(IR_JUMP, makeLabelOperand(body));
    label = exit;
}

static void generateDoWhileExpression(Node* node)
{
    Label* body = makeLabel();
    instruction1(IR_JUMP, makeLabelOperand(body));
    label = body;
    generateStatements(node->children.whileBody);
    Operand* condition = generateExpression(node->children.whileCondition);
    Label* exit = makeLabel();
    instruction2(IR_JUMP_IF_TRUE, condition, makeLabelOperand(exit));
    instruction1(IR_JUMP, makeLabelOperand(body));
    label = exit;
}

static void generateMatchExpression(Node* node)
{
    Operand* value = generateExpression(node->children.matchValue);
    Operand* condition = makeRegister();
    Label* exit = makeLabel();
    Label* start = label;
    Label* defaultLabel = NULL;

    VEC_EACH(node->children.matchArms) {
        Node* arm = VEC_EACH_GET(node->children.matchArms);

        if (arm->type == NODE_MATCH_ARM) {
            Operand* match = generateExpression(arm->children.matchArmExpression);
            instruction3(IR_EQUAL, value, match, condition);
            Label* body = makeLabel();
            instruction2(IR_JUMP_IF_TRUE, condition, makeLabelOperand(body));
            label = body;
            generateExpression(node->children.matchArmBody);
        } else {
            Label* body = makeLabel();
            defaultLabel = body;
            label = body;
            generateExpression(node->children.node);
        }

        instruction1(IR_JUMP, makeLabelOperand(exit));
        label = start;
    }

    instruction1(IR_JUMP, makeLabelOperand(defaultLabel ? defaultLabel : exit));
    label = exit;
}

static Operand* generateExpression(Node* node)
{
    switch (node->type) {
        case NODE_ADD:
            return doubleOperation(node, IR_ADD);
        case NODE_SUBSTRACT:
            return doubleOperation(node, IR_SUBSTRACT);
        case NODE_MULTIPLY:
            return doubleOperation(node, IR_MULTIPLY);
        case NODE_DIVIDE:
            return doubleOperation(node, IR_DIVIDE);
        case NODE_MODULO:
            return doubleOperation(node, IR_MODULO);
        case NODE_AND:
            return doubleOperation(node, IR_AND);
        case NODE_OR:
            return doubleOperation(node, IR_OR);
        case NODE_NEGATE:
            return unaryOperation(node, IR_NEGATE);
        case NODE_NOT:
            return unaryOperation(node, IR_NOT);
        case NODE_INTEGER: {
            Operand* value = makeIntegerOperand(node->children.integer);
            Operand* destination = makeRegister();
            instruction2(IR_MOVE, value, destination);

            return destination;
        }
        case NODE_BOOLEAN: {
            Operand* value = makeIntegerOperand(node->children.boolean);
            Operand* destination = makeRegister();
            instruction2(IR_MOVE, value, destination);

            return destination;
        }
        case NODE_VAR: {
            Operand* value = generateExpression(node->children.varValue);
            Operand* pointer = makeRegister();
            instruction2(IR_ALLOCATE, makeIntegerOperand(4), pointer);
            instruction2(IR_STORE, value, pointer);

            return pointer;
        }
        case NODE_LOAD: {
            Operand* pointer = makeRegister(); // TODO
            Operand* destination = makeRegister();
            instruction2(IR_LOAD, pointer, destination);

            return destination;
        }
        case NODE_WHILE:
            generateWhileExpression(node);
            return NULL;
        case NODE_EQUAL:
            return doubleOperation(node, IR_EQUAL);
        case NODE_NOT_EQUAL:
            return doubleOperation(node, IR_NOT_EQUAL);
        case NODE_LESS:
            return doubleOperation(node, IR_LESS);
        case NODE_LESS_EQUAL:
            return doubleOperation(node, IR_LESS_EQUAL);
        case NODE_GREATER:
            return doubleOperation(node, IR_GREATER);
        case NODE_GREATER_EQUAL:
            return doubleOperation(node, IR_GREATER_EQUAL);
        case NODE_IF:
            generateIfExpression(node);
            return NULL;
        case NODE_DO_WHILE:
            generateDoWhileExpression(node);
            return NULL;
        case NODE_MATCH:
            generateMatchExpression(node);
            return NULL;
    }
}

static void generateStatements(Node* node)
{
    VEC_EACH(node->children.nodes) {
        Node* child = VEC_EACH_GET(node->children.nodes);
        generateExpression(child);
    }
}

static Function* generateFunction(Node* node)
{
    function = safeMalloc(sizeof(Function));
    function->name = node->children.functionName;
    function->argumentCount = 0;
    function->labels = newVector();
    function->nextLabelNumber = 0;
    function->nextRegisterNumber = 0;

    label = makeLabel();
    generateStatements(node->children.functionBody);
    instruction0(IR_RETURN);

    return function;
}

IR* generateIR(Node* node)
{
    IR* ir = makeIR();

    VEC_EACH(node->children.nodes) {
        Node* functionNode = VEC_EACH_GET(node->children.nodes);
        vectorPush(ir->functions, generateFunction(functionNode));
    }

    return ir;
}

static void freeOperand(Operand* operand)
{
    switch (operand->type) {
        case OPERAND_REGISTER:
            free(operand->value.reg);
            break;
    }

    free(operand);
}

static void freeInstruction(Instruction* instruction)
{
    if (instruction->operand1) {
        freeOperand(instruction->operand1);
    }
    
    if (instruction->operand2) {
        freeOperand(instruction->operand2);
    }

    if (instruction->operand3) {
        freeOperand(instruction->operand3);
    }

    free(instruction);
}

static void freeLabel(Label* label)
{
    VEC_EACH(label->instructions) {
        Instruction* instruction = VEC_EACH_GET(label->instructions);
        freeInstruction(instruction);
    }

    free(label);
}

static void freeFunction(Function* function)
{
    VEC_EACH(function->labels) {
        Label* label = VEC_EACH_GET(function->labels);
        freeLabel(label);
    }

    free(function->name);
    free(function);
}

void freeIR(IR* ir)
{
    VEC_EACH(ir->functions) {
        Function* function = VEC_EACH_GET(ir->functions);
        freeFunction(function);
    }

    free(ir);
}
