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

Scanner* scanner;

static Token* makeToken(TokenType type)
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

static char peekAt(int index)
{
    return scanner->module->source[index];
}

static char peek()
{
    return peekAt(scanner->currentIndex);
}

static void advance()
{
    scanner->currentIndex++;
}

static bool isAtEnd()
{
    return peek(scanner) == '\0';
}

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static int convertCharToInteger(char c)
{
    return (int) c - 48;
}

static void back()
{
    scanner->currentIndex--;
}

static char peekNext()
{
    return peekAt(scanner->currentIndex + 1);
}

static bool match(char c)
{
    if (isAtEnd() || peekNext() != c) {
        return false;
    }

    advance();

    return true;
}

static Token* makeNumber()
{
    char c = peek();
    float value = convertCharToInteger(c);
    advance();
    bool hasDot = false;
    int divider = 10;

    while (isDigit(c = peek())) {
        value = hasDot ?
            value + convertCharToInteger(c) / divider :
            value * 10 + convertCharToInteger(c);
        
        if (hasDot) {
            divider *= 10;
        }

        if (!hasDot && match('.')) {
            hasDot = true;
        }

        advance();
    }

    back();
    Token* token = makeToken(hasDot ? TOKEN_FLOAT : TOKEN_INTEGER);
    
    if (hasDot) {
        token->value._float = value;
    } else {
        token->value.integer = (int)value;
    }

    return token;
}

static void skipWhitespaces()
{
    while(true) {
        if (isWhitespace(peek())) {
            advance();

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

static Token* makeKeyword(char* keyword)
{
    switch (*keyword) {
        case 'a':
            if (!strcmp(keyword, "abstract")) {
                return makeToken(TOKEN_ABSTRACT);
            }
            break;
        case 'b':
            if (!strcmp(keyword, "break")) {
                return makeToken(TOKEN_BREAK);
            }
            break;
        case 'c':
            if (!strcmp(keyword, "class")) {
                return makeToken(TOKEN_CLASS);
            }
            if (!strcmp(keyword, "const")) {
                return makeToken(TOKEN_CONST);
            }
            if (!strcmp(keyword, "continue")) {
                return makeToken(TOKEN_CONTINUE);
            }
            break;
        case 'd':
            if (!strcmp(keyword, "do")) {
                return makeToken(TOKEN_DO);
            }
            break;
        case 'e':
            if (!strcmp(keyword, "else")) {
                return makeToken(TOKEN_ELSE);
            }
            if (!strcmp(keyword, "enum")) {
                return makeToken(TOKEN_ENUM);
            }
            break;
        case 'f':
            if (!strcmp(keyword, "for")) {
                return makeToken(TOKEN_FOR);
            }
            break;
        case 'i':
            if (!strcmp(keyword, "if")) {
                return makeToken(TOKEN_IF);
            }
            if (!strcmp(keyword, "interface")) {
                return makeToken(TOKEN_INTERFACE);
            }
            break;
        case 'l':
            if (!strcmp(keyword, "loop")) {
                return makeToken(TOKEN_LOOP);
            }
            break;
        case 'r':
            if (!strcmp(keyword, "return")) {
                return makeToken(TOKEN_RETURN);
            }
            break;
        case 'v':
            if (!strcmp(keyword, "var")) {
                return makeToken(TOKEN_VAR);
            }
            break;
        case 'w':
            if (!strcmp(keyword, "while")) {
                return makeToken(TOKEN_WHILE);
            }
            break;
    }

    return NULL;
}

static Token* makeIdentifier()
{
    StringBuilder* builder = newStringBuilder();

    while (isAlpha(peek()) || isDigit(peek())) {
        addStringBuilder(builder, peek());
        advance();
    }

    back();

    char* identifier = buildStringBuilder(builder);
    freeStringBuilder(builder);
    Token* token = makeKeyword(identifier);

    if (token != NULL) {
        return token;
    }

    token = makeToken(TOKEN_IDENTIFIER);
    token->value.string = identifier;

    return token;
}

static bool expect(char c, char* message)
{
    if (!match(c)) {
        addErrorAt(scanner->module, scanner->currentIndex + 1, scanner->currentIndex + 2, message, peekNext());

        return false;
    }

    return true;
}

static Token* makeChar()
{
    advance();
    Token* token = makeToken(TOKEN_CHAR);
    token->value.c = peek();
    expect('\'', "A character value must be closed with \"'\" but received \"%c\".");

    return token;
}

static Token* makeString()
{
    advance();
    StringBuilder* builder = newStringBuilder();
    char c;

    while (c = peek(scanner) != '"') {
        addStringBuilder(builder, c);
        advance();

        if (isAtEnd()) {
            addErrorAt(scanner->module, scanner->currentIndex, scanner->currentIndex + 1, "A string value must be closed with '\"' but it's the end of the file.");
            back();

            return makeToken(TOKEN_NONE);
        }
    }

    Token* token = makeToken(TOKEN_STRING);
    token->value.string = buildStringBuilder(builder);
    freeStringBuilder(builder);

    return token;
}

static Token* getToken()
{
    skipWhitespaces();

    if (isAtEnd()) {
        return makeToken(TOKEN_EOF);
    }

    char c = peek();

    if (isAlpha(c)) {
        return makeIdentifier();
    }

    if (isDigit(c)) {
        return makeNumber();
    }

    switch (c) {
        case '+': return makeToken(match('=') ? TOKEN_PLUS_EQUAL : TOKEN_PLUS);
        case '-': return makeToken(match('=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS);
        case '*': return makeToken(match('=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
        case '/': return makeToken(match('/') ? TOKEN_NONE : match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMILICON);
        case '=': return makeToken(match('=') ? TOKEN_DOUBLE_EQUAL : TOKEN_EQUAL);
        case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '.': return makeToken(TOKEN_DOT);
        case ',': return makeToken(TOKEN_COMMA);
        case '^': return makeToken(TOKEN_CIRCUMFLEX);
        case '&': return makeToken(expect('&', "Expect \"&&\" but received \"&%c\".") ? TOKEN_DOUBLE_AMPERSAND : TOKEN_NONE);
        case '|': return makeToken(expect('|', "Expect \"||\" but received \"|%c\".") ? TOKEN_DOUBLE_PIPE : TOKEN_NONE);
        case '?': return makeToken(TOKEN_QUESTION_MARK);
        case '%': return makeToken(match('=') ? TOKEN_MODULO_EQUAL : TOKEN_MODULO);
        case '[': return makeToken(TOKEN_LEFT_BRACKET);
        case ']': return makeToken(TOKEN_RIGHT_BRACKET);
        case '\'': return makeChar();
        case '"': return makeString();
        case '#': return makeToken(TOKEN_HASHTAG);
    }

    addErrorAt(scanner->module, scanner->currentIndex, scanner->currentIndex + 1, "Unexpected character \"%c\".", c);

    return makeToken(TOKEN_NONE);
}

Vector* scan(Module* module)
{
    scanner = newScanner(module);

    while (!isAtEnd()) {
        Token* token = getToken();
        advance();
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
    
    pushVector(scanner->tokens, makeToken(TOKEN_EOF));
    Vector* tokens = scanner->tokens;
    free(scanner);

    return tokens;
}
