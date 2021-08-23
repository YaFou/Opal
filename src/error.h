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
#define E015   "E015 - Expect an expression."
#define E016   "E016 - Expect variable name."
#define E017   "E017 - Expect \"=\" to assign the variable."
#define E018   "E018 - Expect member name."
#define E019   "E019 - Expect \"while\" to set the loop condition."
#define E020   "E020 - Expect \";\" after a do while loop condition."
#define E021   "E021 - Expect \":\" to set the case block."
#define E022   "E022 - Expect \",\" after an inline expression of a match arm."
#define E023   "E023 - Expect \":\" to set the else expression of a ternary expression."

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
