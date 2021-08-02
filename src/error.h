#ifndef OPAL_ERROR_H
#define OPAL_ERROR_H

void throwFailedAlloc();
void throwFatal(char* message, ...);
void addError(char* message, ...);
void throwErrors();

#endif
