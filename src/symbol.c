#include "symbol.h"
#include "util.h"

Environment* newEnvironment()
{
    Environment* environment = safeMalloc(sizeof(Environment));
    environment->variables = newMap();

    return environment;
}

Variable* newEnvironmentVariable(Environment* environment, char* name, char* type)
{
    Variable* variable = safeMalloc(sizeof(Variable));
    variable->type = type;
    setMap(environment->variables, name, variable);

    return variable;
}

void freeEnvironment(Environment* environment)
{
    freeMap(environment->variables);
    free(environment);
}

Variable* getEnvironmentVariable(Environment* environment, char* name)
{
    return getMap(environment->variables, name);
}
