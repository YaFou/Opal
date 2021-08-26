#include "types.h"
#include "memory.h"
#include "error.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define TYPE_INTEGER "<integer>"
#define TYPE_BOOLEAN "<boolean>"
#define TYPE_VOID    "<void>"

typedef struct Env {
    struct Env* parent;
    Map* vars;
    Type* returnType;
    Type* functionReturnType;
    Type* matchType;
} Env;

typedef struct {
    Type* type;
} Variable;

Env* env = NULL;
Module* module;

static Env* newEnv()
{
    Env* env = safeMalloc(sizeof(Env));
    env->parent = NULL;
    env->vars = newMap();
    env->returnType = NULL;
    env->functionReturnType = NULL;
    env->matchType = NULL;

    return env;
}

static void freeEnv(Env* env)
{
    freeMap(env->vars);
    free(env);
}

static void openFrame()
{
    Env* new = newEnv();

    if (env->functionReturnType) {
        new->functionReturnType = env->functionReturnType;
    }

    new->parent = env;
    env = new;
}

static void closeFrame()
{
    Env* current = env;
    env = current->parent;
    freeEnv(current);
}

static Type* makeType(const char* name, bool nullable)
{
    Type* type = safeMalloc(sizeof(Type));
    type->name = name;
    type->nullable = nullable;

    return type;
}

static Type* integerType()
{
    return makeType(TYPE_INTEGER, false);
}

static Type* booleanType()
{
    return makeType(TYPE_BOOLEAN, false);
}

static Type* voidType()
{
    return makeType(TYPE_VOID, false);
}

static void addErrorAtNode(Node* node, const char* message, ...)
{
    va_list ap;
    va_start(ap, message);
    char buffer[BUFFER_SIZE];
    vsprintf(buffer, message, ap);
    va_end(ap);

    addErrorAt(module, node->startIndex, node->endIndex, buffer);
}

static bool isSameType(Type* type, const char* name, bool allowNullable)
{
    if (name == TYPE_VOID) {
        return false;
    }

    return !strcmp(type->name, name) && type->nullable == allowNullable;
}

static bool isVoid(Node* node)
{
    return !strcmp(node->valueType->name, TYPE_VOID);
}

static bool isSameTypes(Type* type1, Type* type2)
{
    return !strcmp(type1->name, type2->name) && type1->nullable == type2->nullable;
}

static bool isSameNodeTypes(Node* node1, Node* node2)
{
    return isSameTypes(node1->valueType, node2->valueType);
}

static void doubleOperation(Node* node, const char* type)
{
    Type* left = node->children.left->valueType;
    Type* right = node->children.right->valueType;
    
    if (!isSameType(left, type, false) || !isSameType(right, type, false)) {
        addErrorAtNode(node, E014_2, type, left->name, right->name);
    }
}

static void unary(Node* node, const char* type)
{
    Type* inner = node->children.node->valueType;

    if (!isSameType(inner, type, false)) {
        addErrorAtNode(node->children.node, E014_1, type, inner->name);
    }

    node->valueType = makeType(type, false);
}

static void condition(Node* node)
{
    if (!isSameType(node->valueType, TYPE_BOOLEAN, false)) {
        addErrorAtNode(node, E028, TYPE_BOOLEAN, node->valueType->name);
    }
}

static void newVariable(char* name, Type* type)
{
    Variable* variable = safeMalloc(sizeof(Variable));
    variable->type = type;
    mapSet(env->vars, name, variable);
}

static Variable* getVariableFromEnv(Env* env, char* name)
{
    Variable* variable = mapGet(env->vars, name);

    if (variable) {
        return variable;
    }

    if (env->parent) {
        return getVariableFromEnv(env->parent, name);
    }

    return NULL;
}

static Variable* getVariable(char* name)
{
    return getVariableFromEnv(env, name);
}

static void checkNode(Node* node)
{
    switch (node->type) {
        case NODE_FUNCTION:
            env->functionReturnType = node->valueType = voidType();
            checkNode(node->children.functionBody);
            break;
        case NODE_MODULE: {
            VEC_EACH(node->children.nodes) {
                Node* child = VEC_EACH_GET(node->children.nodes);
                checkNode(child);
            }
            break;
        }
        case NODE_STATEMENTS: {
            openFrame();

            VEC_EACH(node->children.nodes) {
                Node* child = VEC_EACH_GET(node->children.nodes);
                checkNode(child);
            }

            Type* returnType = env->returnType != NULL ? env->returnType : voidType();
            closeFrame();
            // TODO
            node->valueType = returnType;
            break;
        }
        case NODE_VAR: {
            checkNode(node->children.varValue);
            char* name = node->children.varName;
            Type* varValue = node->children.varValue->valueType;

            if (!mapHas(env->vars, name)) {
                newVariable(name, varValue);
            } else {
                addErrorAtNode(node, E026, name);
            }

            node->valueType = varValue;
            break;
        }
        case NODE_ADD:
        case NODE_SUBSTRACT:
        case NODE_MULTIPLY:
        case NODE_DIVIDE:
        case NODE_MODULO:
        case NODE_POWER:
        case NODE_AND:
        case NODE_OR:
        case NODE_EQUAL:
        case NODE_NOT_EQUAL:
        case NODE_LESS:
        case NODE_LESS_EQUAL:
        case NODE_GREATER:
        case NODE_GREATER_EQUAL:
        case NODE_ASSIGNMENT:
        case NODE_ADD_ASSIGNMENT:
        case NODE_SUBSTRACT_ASSIGNMENT:
        case NODE_MULTIPLY_ASSIGNMENT:
        case NODE_POWER_ASSIGNMENT:
        case NODE_DIVIDE_ASSIGNMENT:
        case NODE_MODULO_ASSIGNMENT: {
            checkNode(node->children.left);
            checkNode(node->children.right);
            Type* left = node->children.left->valueType;
            Type* right = node->children.right->valueType;

            switch (node->type) {
                case NODE_ADD:
                case NODE_SUBSTRACT:
                case NODE_MULTIPLY:
                case NODE_DIVIDE:
                case NODE_MODULO:
                case NODE_POWER:
                case NODE_ADD_ASSIGNMENT:
                case NODE_SUBSTRACT_ASSIGNMENT:
                case NODE_MULTIPLY_ASSIGNMENT:
                case NODE_POWER_ASSIGNMENT:
                case NODE_DIVIDE_ASSIGNMENT:
                case NODE_MODULO_ASSIGNMENT: {
                    doubleOperation(node, TYPE_INTEGER);
                    node->valueType = integerType();
                    break;
                }
                case NODE_LESS:
                case NODE_LESS_EQUAL:
                case NODE_GREATER:
                case NODE_GREATER_EQUAL: {
                    doubleOperation(node, TYPE_INTEGER);
                    node->valueType = booleanType();
                    break;
                }
                case NODE_AND:
                case NODE_OR:
                    doubleOperation(node, TYPE_BOOLEAN);
                    node->valueType = booleanType();
                    break;
                case NODE_ASSIGNMENT: {
                    if (!isSameType(right, left->name, left->nullable)) {
                        addErrorAtNode(node, E024, left->name, right->name);
                    }

                    node->valueType = left;
                    break;
                }
                case NODE_EQUAL:
                case NODE_NOT_EQUAL: {
                    if (!isSameType(right, left->name, left->nullable)) {
                        addErrorAtNode(node, E025, left->name, right->name);
                    }

                    node->valueType = left;
                    break;
                }
            }

            break;
        }
        case NODE_NEGATE:
        case NODE_NOT:
        case NODE_PRE_INCREMENT:
        case NODE_PRE_DECREMENT:
        case NODE_POST_INCREMENT:
        case NODE_POST_DECREMENT:
        case NODE_LOOP: {
            checkNode(node->children.node);

            switch (node->type) {
                case NODE_NEGATE:
                case NODE_PRE_INCREMENT:
                case NODE_PRE_DECREMENT:
                case NODE_POST_INCREMENT:
                case NODE_POST_DECREMENT:
                    unary(node, TYPE_INTEGER);
                    break;
                case NODE_NOT:
                    unary(node, TYPE_BOOLEAN);
                    break;
            }

            break;
        }
        case NODE_MEMBER:
            checkNode(node->children.memberValue);
            // TODO
            break;
        case NODE_IF: {
            Node* ifCondition = node->children.ifCondition;
            Node* ifBody = node->children.ifBody;
            Node* elseBody = node->children.elseBody;
            checkNode(ifCondition);
            condition(ifCondition);
            checkNode(ifBody);

            if (elseBody) {
                checkNode(elseBody);

                if (!isSameNodeTypes(ifBody, elseBody)) {
                    addErrorAtNode(node, E030, ifBody->valueType->name, elseBody->valueType->name);
                }
            } else if (!isVoid(ifBody)) {
                addErrorAtNode(node, E029, ifBody->valueType->name);
            }

            node->valueType = ifBody->valueType;
            
            break;
        }
        case NODE_WHILE:
        case NODE_DO_WHILE:
            checkNode(node->children.whileCondition);
            condition(node->children.whileCondition);
            checkNode(node->children.whileBody);
            node->valueType = node->children.whileBody->valueType;
            break;
        case NODE_MATCH: {
            Node* matchValue = node->children.matchValue;
            checkNode(matchValue);
            Type* matchType = NULL;

            if (isVoid(matchValue)) {
                addErrorAtNode(matchValue, E031, matchValue->valueType->name);
            } else {
                matchType = matchValue->valueType;
            }

            openFrame();
            env->matchType = matchType;
            Type* returnType = NULL;

            VEC_EACH(node->children.matchArms) {
                Node* arm = VEC_EACH_GET(node->children.matchArms);
                checkNode(arm);

                if (!returnType) {
                    returnType = arm->valueType;
                } else if (!isSameTypes(returnType, arm->valueType)) {
                    addErrorAtNode(arm->type == NODE_MATCH_ARM ? arm->children.matchArmBody : arm->children.node, E032, returnType->name, arm->valueType->name);
                }
            }

            closeFrame();
            node->valueType = returnType != NULL ? returnType : voidType();

            break;
        }
        case NODE_MATCH_ARM: {
            Node* expression = node->children.matchArmExpression;
            checkNode(expression);
            Type* matchType = env->matchType;

            if (matchType && !isSameType(expression->valueType, matchType->name, matchType->nullable)) {
                addErrorAtNode(expression, E024, matchType->name, expression->valueType->name);
            }

            Node* body = node->children.matchArmBody;
            checkNode(body);
            node->valueType = body->valueType;

            break;
        }
        case NODE_INTEGER: {
            node->valueType = integerType();
            break;
        }
        case NODE_BOOLEAN: {
            node->valueType = booleanType();
            break;
        }
        case NODE_LOAD: {
            Variable* variable = getVariable(node->children.string);

            if (!variable) {
                addErrorAtNode(node, E027, node->children.string);
                node->valueType = voidType();

                break;
            }

            node->valueType = variable->type;
            break;
        }
        case NODE_RETURN: {
            if (node->children.node) {
                checkNode(node->children.node);
            }

            printf("%d\n", env->functionReturnType);
            if (!isSameTypes(env->functionReturnType, node->children.node->valueType)) {
                addErrorAtNode(node, E024, env->functionReturnType->name, node->children.node->valueType->name);
            }

            node->valueType = voidType();
        }
        case NODE_MATCH_ARM_DEFAULT: {
            Node* body = node->children.node;
            checkNode(body);
            node->valueType = body->valueType;

            break;
        }
        case NODE_IMPLICIT_RETURN: {
            checkNode(node->children.node);
            env->returnType = node->children.node->valueType;
        }
    }
}

void checkTypes(Module* module_, Node* node)
{
    module = module_;
    env = newEnv();
    checkNode(node);
    freeEnv(env);
}
