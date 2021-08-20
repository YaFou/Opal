#ifndef OPAL_ERROR_H
#define OPAL_ERROR_H

#include "module.h"
#include <stdbool.h>

// ERROR CODES
#define E001   "E001 - Failed to open \"%s\"."
#define E002   "E002 - Failed to read \"%s\"."
#define E003   "E003 - Failed to close \"%s\"."
#define E004   "E004 - Unexpected character \"%c\"."
#define E005   "E005 - Expect \"||\" but received \"|%c\"."
#define E006   "E006 - Expect a character but it's the end of the file."
#define E007_1 "E007 - Expect \"'\" but received \"%c\"."
#define E007_2 "E007 - Expect \"'\" but it's the end of the file."
#define E008   "E008 - Expect '\"' but it's end of the file."
#define E009   "E009 - Expect \"&&\" but received \"&%c\"."
#define E010   "E010 - Expect \"(\" for parameters."
#define E011   "E011 - Expect function name."
#define E012   "E012 - An opening \"(\", \"{\" or \"[\" must be closed."
#define E013   "E013 - Expect \"{\" to start a statement body."
#define E014   "E014 - Expect \";\" after a statement."
#define E015   "E015 - Expect an expression."
#define E016   "E016 - Expect variable name."
#define E017   "E017 - Expect \"=\" to assign the variable."

// FATAL CODES
#define F001 "F001 - Failed to allocate memory."

// WARNING CODES
#define W001 "W001 - Unnecessary \";\"."

void throwFatal(const char* message);
void throwError(const char* message, ...);
void addErrorAt(Module* module, int startIndex, int endIndex, const char* message, ...);
void throwErrors();
bool hasErrors();
void addWarningAt(Module* module, int startIndex, int endIndex, const char* message, ...);
void throwWarnings();

#endif
