#include "scan.h"
#include "util.h"
#include <string.h>
#include "stringbuilder.h"
#include "error.h"

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
    token->endIndex = scanner->currentIndex + 1;
    
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

static char getCharAt(Scanner* scanner, int index)
{
    return scanner->module->source[index];
}

static char getChar(Scanner* scanner)
{
    return getCharAt(scanner, scanner->currentIndex);
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

static char getNextChar(Scanner* scanner)
{
    return getCharAt(scanner, scanner->currentIndex + 1);
}

static bool isCharMatch(Scanner* scanner, char c)
{
    if (isAtEnd(scanner) || getNextChar(scanner) != c) {
        return false;
    }

    advance(scanner);

    return true;
}

static Token* makeNumber(Scanner* scanner)
{
    char c = getChar(scanner);
    float value = convertCharToInteger(c);
    advance(scanner);
    bool hasDot = false;
    int divider = 10;

    while (isDigit(c = getChar(scanner))) {
        value = hasDot ?
            value + convertCharToInteger(c) / divider :
            value * 10 + convertCharToInteger(c);
        
        if (hasDot) {
            divider *= 10;
        }

        if (!hasDot && isCharMatch(scanner, '.')) {
            hasDot = true;
        }

        advance(scanner);
    }

    back(scanner);
    Token* token = makeToken(scanner, hasDot ? TOKEN_FLOAT : TOKEN_INTEGER);
    
    if (hasDot) {
        token->value._float = value;
    } else {
        token->value.integer = (int)value;
    }

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

static bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

static Token* makeKeyword(Scanner* scanner, char* keyword)
{
    switch (*keyword) {
        case 'a':
            if (!strcmp(keyword, "abstract")) {
                return makeToken(scanner, TOKEN_ABSTRACT);
            }
            break;
        case 'b':
            if (!strcmp(keyword, "break")) {
                return makeToken(scanner, TOKEN_BREAK);
            }
            break;
        case 'c':
            if (!strcmp(keyword, "class")) {
                return makeToken(scanner, TOKEN_CLASS);
            }
            if (!strcmp(keyword, "const")) {
                return makeToken(scanner, TOKEN_CONST);
            }
            if (!strcmp(keyword, "continue")) {
                return makeToken(scanner, TOKEN_CONTINUE);
            }
            break;
        case 'd':
            if (!strcmp(keyword, "do")) {
                return makeToken(scanner, TOKEN_DO);
            }
            break;
        case 'e':
            if (!strcmp(keyword, "else")) {
                return makeToken(scanner, TOKEN_ELSE);
            }
            if (!strcmp(keyword, "enum")) {
                return makeToken(scanner, TOKEN_ENUM);
            }
            break;
        case 'f':
            if (!strcmp(keyword, "for")) {
                return makeToken(scanner, TOKEN_FOR);
            }
            break;
        case 'i':
            if (!strcmp(keyword, "if")) {
                return makeToken(scanner, TOKEN_IF);
            }
            if (!strcmp(keyword, "interface")) {
                return makeToken(scanner, TOKEN_INTERFACE);
            }
            break;
        case 'l':
            if (!strcmp(keyword, "loop")) {
                return makeToken(scanner, TOKEN_LOOP);
            }
            break;
        case 'r':
            if (!strcmp(keyword, "return")) {
                return makeToken(scanner, TOKEN_RETURN);
            }
            break;
        case 'v':
            if (!strcmp(keyword, "var")) {
                return makeToken(scanner, TOKEN_VAR);
            }
            break;
        case 'w':
            if (!strcmp(keyword, "while")) {
                return makeToken(scanner, TOKEN_WHILE);
            }
            break;
    }

    return NULL;
}

static Token* makeIdentifier(Scanner* scanner)
{
    StringBuilder* builder = newStringBuilder();

    while (isAlpha(getChar(scanner)) || isDigit(getChar(scanner))) {
        addStringBuilder(builder, getChar(scanner));
        advance(scanner);
    }

    back(scanner);

    char* identifier = buildStringBuilder(builder);
    freeStringBuilder(builder);
    Token* token = makeKeyword(scanner, identifier);

    if (token != NULL) {
        return token;
    }

    token = makeToken(scanner, TOKEN_IDENTIFIER);
    token->value.string = identifier;

    return token;
}

static bool expectChar(Scanner* scanner, char c, char* message)
{
    if (!isCharMatch(scanner, c)) {
        addErrorAt(scanner->module, scanner->currentIndex + 1, scanner->currentIndex + 2, message, getNextChar(scanner));

        return false;
    }

    return true;
}

static Token* makeChar(Scanner* scanner)
{
    advance(scanner);
    Token* token = makeToken(scanner, TOKEN_CHAR);
    token->value.c = getChar(scanner);
    expectChar(scanner, '\'', "A character value must be closed with \"'\" but received \"%c\".");

    return token;
}

static Token* makeString(Scanner* scanner)
{
    advance(scanner);
    StringBuilder* builder = newStringBuilder();
    char c;

    while (c = getChar(scanner) != '"') {
        addStringBuilder(builder, c);
        advance(scanner);

        if (isAtEnd(scanner)) {
            addErrorAt(scanner->module, scanner->currentIndex, scanner->currentIndex + 1, "A string value must be closed with '\"' but it's the end of the file.");
            back(scanner);

            return makeToken(scanner, TOKEN_NONE);
        }
    }

    Token* token = makeToken(scanner, TOKEN_STRING);
    token->value.string = buildStringBuilder(builder);
    freeStringBuilder(builder);

    return token;
}

static Token* getToken(Scanner* scanner)
{
    skipWhitespaces(scanner);

    if (isAtEnd(scanner)) {
        return makeToken(scanner, TOKEN_EOF);
    }

    char c = getChar(scanner);

    if (isAlpha(c)) {
        return makeIdentifier(scanner);
    }

    if (isDigit(c)) {
        return makeNumber(scanner);
    }

    switch (c) {
        case '+': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_PLUS_EQUAL : TOKEN_PLUS);
        case '-': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS);
        case '*': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
        case '/': return makeToken(scanner, isCharMatch(scanner, '/') ? TOKEN_NONE : isCharMatch(scanner, '=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
        case '(': return makeToken(scanner, TOKEN_LEFT_PAREN);
        case ')': return makeToken(scanner, TOKEN_RIGHT_PAREN);
        case '{': return makeToken(scanner, TOKEN_LEFT_BRACE);
        case '}': return makeToken(scanner, TOKEN_RIGHT_BRACE);
        case ';': return makeToken(scanner, TOKEN_SEMILICON);
        case '=': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_DOUBLE_EQUAL : TOKEN_EQUAL);
        case '!': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '<': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '.': return makeToken(scanner, TOKEN_DOT);
        case ',': return makeToken(scanner, TOKEN_COMMA);
        case '^': return makeToken(scanner, TOKEN_CIRCUMFLEX);
        case '&': return makeToken(scanner, expectChar(scanner, '&', "Expect \"&&\" but received \"&%c\".") ? TOKEN_DOUBLE_AMPERSAND : TOKEN_NONE);
        case '|': return makeToken(scanner, expectChar(scanner, '|', "Expect \"||\" but received \"|%c\".") ? TOKEN_DOUBLE_PIPE : TOKEN_NONE);
        case '?': return makeToken(scanner, TOKEN_QUESTION_MARK);
        case '%': return makeToken(scanner, isCharMatch(scanner, '=') ? TOKEN_MODULO_EQUAL : TOKEN_MODULO);
        case '[': return makeToken(scanner, TOKEN_LEFT_BRACKET);
        case ']': return makeToken(scanner, TOKEN_RIGHT_BRACKET);
        case '\'': return makeChar(scanner);
        case '"': return makeString(scanner);
        case '#': return makeToken(scanner, TOKEN_HASHTAG);
    }

    addErrorAt(scanner->module, scanner->currentIndex, scanner->currentIndex + 1, "Unexpected character \"%c\".", c);

    return makeToken(scanner, TOKEN_NONE);
}

Vector* scan(Module* module)
{
    Scanner* scanner = newScanner(module);

    while (!isAtEnd(scanner)) {
        Token* token = getToken(scanner);
        advance(scanner);
        scanner->startIndex = scanner->currentIndex;

        if (token->type == TOKEN_NONE || hasErrors()) {
            if (token->type == TOKEN_EOF) {
                break;
            }

            continue;
        }

        if (token->type == TOKEN_EOF) {
            break;
        }

        pushVector(scanner->tokens, token);
    }
    
    pushVector(scanner->tokens, makeToken(scanner, TOKEN_EOF));
    Vector* tokens = scanner->tokens;
    free(scanner);

    return tokens;
}
