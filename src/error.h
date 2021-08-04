#ifndef OPAL_ERROR_H
#define OPAL_ERROR_H

#include "module.h"
#include <stdbool.h>

void throwFailedAlloc();
void throwFatal(char* message, ...);
void addError(char* message, ...);
void throwErrors();
void addErrorAt(Module* module, int startIndex, int endIndex, char* message, ...);
bool hasErrors();

#endif
