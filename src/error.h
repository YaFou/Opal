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
#define E014_1 "E014 - This operation supports type \"%s\" but received type \"%s\"."
#define E014_2 "E014 - This operation supports type \"%s\" but received types \"%s\" and \"%s\"."
#define E015   "E015 - Expect an expression."
#define E016   "E016 - Expect variable name."
#define E017   "E017 - Expect \"=\" to assign the variable."
#define E018   "E018 - Expect member name."
#define E019   "E019 - Expect \"while\" to set the loop condition."
#define E020   "E020 - Expect \";\" after a do while loop condition."
#define E021   "E021 - Expect \":\" to set the case block."
#define E022   "E022 - Expect \",\" after an inline expression of a match arm."
#define E023   "E023 - Expect \":\" to set the else expression of a ternary expression."
#define E024   "E024 - Expect type \"%s\" but received type \"%s\"."
#define E025   "E025 - Types \"%s\" and \"%s\" are incompatible."
#define E026   "E026 - Variable \"%s\" already exists."
#define E027   "E027 - Variable \"%s\" doesn't exist."
#define E028   "E028 - Conditon must be a type of \"%s\" but received type of \"%s\"."
#define E029   "E029 - Expect else block which returns type \"%s\" due to the if block."
#define E030   "E030 - Expect type \"%s\" in the else block due to the if block but received \"%s\"."
#define E031   "E031 - Expect a match value to have a type but received \"%s\"."
#define E032   "E032 - Expect type \"%s\" due to other match arms but received type \"%s\"."

// FATAL CODES
#define F001 "F001 - Failed to allocate memory."

// WARNING CODES
#define W001 "W001 - Unnecessary \";\"."
#define W002 "W002 - Unnecessary unary \"+\"."

void throwFatal(const char* message);
void throwError(const char* message, ...);
void addErrorAt(Module* module, int startIndex, int endIndex, const char* message, ...);
void throwErrors();
bool hasErrors();
void addWarningAt(Module* module, int startIndex, int endIndex, const char* message, ...);
void throwWarnings();

#endif
