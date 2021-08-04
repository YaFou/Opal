#include "scan.h"
#include "error.h"
#include "util.h"
#include <stdbool.h>

typedef struct {
    Module* module;
    int startIndex;
    int currentIndex;
    Vector* tokens;
} Scanner;

static Token* makeToken(Scanner* scanner, TokenType type)
{
    Token* token = safeMalloc(sizeof(Token));
    token->type = type;
    token->module = scanner->module;
    token->startIndex = scanner->startIndex;
    token->endIndex = scanner->currentIndex;
    
    return token;
}

static Scanner* newScanner(Module* module)
{
    Scanner* scanner = safeMalloc(sizeof(Scanner));
    scanner->module = module;
    scanner->startIndex = 0;
    scanner->currentIndex = 0;
    scanner->tokens = newVector();

    return scanner;
}

static char getChar(Scanner* scanner)
{
    return scanner->module->source[scanner->currentIndex];
}

static void advance(Scanner* scanner)
{
    scanner->currentIndex++;
}

static bool isAtEnd(Scanner* scanner)
{
    return getChar(scanner) == '\0';
}

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static int convertCharToInteger(char c)
{
    return (int) c - 48;
}

static void back(Scanner* scanner)
{
    scanner->currentIndex--;
}

static Token* makeNumber(Scanner* scanner)
{
    char c = getChar(scanner);
    int value = convertCharToInteger(c);
    advance(scanner);

    while (isDigit(c = getChar(scanner))) {
        value = value * 10 + convertCharToInteger(c);
        advance(scanner);
    }

    back(scanner);
    Token* token = makeToken(scanner, TOKEN_INTEGER);
    token->value.integer = value;

    return token;
}

static void skipWhitespaces(Scanner* scanner)
{
    while(true) {
        if (isWhitespace(getChar(scanner))) {
            advance(scanner);

            continue;
        }

        return;
    }
}

static Token* getToken(Scanner* scanner)
{
    skipWhitespaces(scanner);

    if (isAtEnd(scanner)) {
        return makeToken(scanner, TOKEN_EOF);
    }

    char c = getChar(scanner);

    if (isDigit(c)) {
        return makeNumber(scanner);
    }

    switch (c) {
        case '+': return makeToken(scanner, TOKEN_PLUS);
        case '-': return makeToken(scanner, TOKEN_MINUS);
        case '*': return makeToken(scanner, TOKEN_STAR);
        case '/': return makeToken(scanner, TOKEN_SLASH);
        case '(': return makeToken(scanner, TOKEN_LEFT_PAREN);
        case ')': return makeToken(scanner, TOKEN_RIGHT_PAREN);
        case '{': return makeToken(scanner, TOKEN_LEFT_BRACE);
        case '}': return makeToken(scanner, TOKEN_RIGHT_BRACE);
        case ';': return makeToken(scanner, TOKEN_SEMILICON);
    }

    addErrorAt(scanner->module, scanner->currentIndex, scanner->currentIndex + 1, "Unexpected character \"%c\".", c);

    return makeToken(scanner, TOKEN_ERROR);
}

Vector* scan(Module* module)
{
    Scanner* scanner = newScanner(module);

    while (!isAtEnd(scanner)) {
        Token* token = getToken(scanner);
        advance(scanner);
        scanner->startIndex = scanner->currentIndex;

        if (token->type == TOKEN_ERROR || hasErrors()) {
            if (token->type == TOKEN_EOF) {
                break;
            }

            continue;
        }

        pushVector(scanner->tokens, token);

        if (token->type == TOKEN_EOF) {
            break;
        }
    }

    // Token* eofToken = makeToken(scanner, TOKEN_EOF);
    // pushVector(scanner->tokens, eofToken);
    Vector* tokens = scanner->tokens;
    free(scanner);

    return tokens;
}
