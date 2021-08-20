#include "scan.h"
#include <stdlib.h>
#include "memory.h"
#include <stdbool.h>
#include "error.h"
#include <string.h>

typedef struct {
    Module* module;
    Vector* tokens;
    int startIndex;
    int currentIndex;
} Scanner;

Scanner* scanner;

static Scanner* newScanner(Module* module)
{
    Scanner* scanner = safeMalloc(sizeof(Scanner));
    scanner->module = module;
    scanner->tokens = newVector();
    scanner->startIndex = 0;
    scanner->currentIndex = 0;

    return scanner;
}

static void freeScanner(Scanner* scanner)
{
    free(scanner);
}

static char peekAt(int index)
{
    return scanner->module->source[index];
}

static char peek()
{
    return peekAt(scanner->currentIndex);
}

static char peekNext()
{
    return peekAt(scanner->currentIndex + 1);
}

static bool isAtEnd()
{
    return peek() == '\0';
}

static void advance()
{
    scanner->currentIndex++;
}

static void back()
{
    scanner->currentIndex--;
}

static void skipWhitespaces()
{
    while (true) {
        char c = peek();

        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                advance();
                break;
            default:
                return;
        }
    }
}

static Token* makeToken(TokenType_ type)
{
    Token* token = safeMalloc(sizeof(Token));
    token->module = scanner->module;
    token->startIndex = scanner->startIndex;
    token->endIndex = scanner->currentIndex + 1;
    token->type = type;

    return token;
}

static bool match(char c)
{
    if (peekNext() == c) {
        advance();

        return true;
    }

    return false;
}

static void addErrorAtCurrent(const char* message, char c)
{
    addErrorAt(scanner->module, scanner->currentIndex, scanner->currentIndex + 1, message, c);
}

static bool expect(char c, const char* message)
{
    if (!match(c)) {
        advance();
        addErrorAtCurrent(message, peek());

        return false;
    }

    return true;
}

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static int charToInt(char c)
{
    return (int) c - 48;
}

static Token* number(bool isFloat)
{
    if (isFloat) {
        advance();
    }

    char c = peek();
    float value = charToInt(c);

    if (isFloat) {
        value /= 10;
    }

    bool hasDot = isFloat;
    int divider = isFloat ? 100 : 10;

    while (isDigit(c = peekNext())) {
        advance();
        value = hasDot ? value + (float) charToInt(c) / divider : value * 10 + charToInt(c);

        if (hasDot) {
            divider *= 10;
        } else if (match('.')) {
            hasDot = true;
        }
    }

    Token* token = makeToken(hasDot ? TOKEN_FLOAT : TOKEN_INTEGER);

    if (hasDot) {
        token->value._float = value;
    } else {
        token->value.integer = (int) value;
    }

    return token;
}

static bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

static Token* keyword(const char* keyword)
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
            if (!strcmp(keyword, "constructor")) {
                return makeToken(TOKEN_CONSTRUCTOR);
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
            if (!strcmp(keyword, "false")) {
                return makeToken(TOKEN_FALSE);
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
        case 'm':
            if (!strcmp(keyword, "match")) {
                return makeToken(TOKEN_MATCH);
            }
            break;
        case 'n':
            if (!strcmp(keyword, "new")) {
                return makeToken(TOKEN_NEW);
            }
            if (!strcmp(keyword, "null")) {
                return makeToken(TOKEN_NULL);
            }
            break;
        case 'r':
            if (!strcmp(keyword, "return")) {
                return makeToken(TOKEN_RETURN);
            }
            break;
        case 's':
            if (!strcmp(keyword, "static")) {
                return makeToken(TOKEN_STATIC);
            }
            break;
        case 't':
            if (!strcmp(keyword, "true")) {
                return makeToken(TOKEN_TRUE);
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

static Token* identifier()
{
    StringBuilder* builder = newSB();
    addSB(builder, peek());
    advance();
    char c = peek();

    while (isAlpha(c) || c == '_' || isDigit(c)) {
        addSB(builder, c);
        advance();
        c = peek();
    }

    back();
    const char* identifier = buildSB(builder);
    freeSB(builder);
    Token* kword = keyword(identifier);

    if (kword != NULL) {
        return kword;
    };

    Token* token = makeToken(TOKEN_IDENTIFIER);
    token->value.string = identifier;

    return token;
}

static Token* char_()
{
    advance();
    
    if (isAtEnd()) {
        addErrorAtCurrent(E006, peek());
        back();

        return NULL;
    }

    char c = peek();

    if (peekNext() == '\0') {
        advance();
        addErrorAtCurrent(E007_2, peek());
        back();

        return NULL;
    }
    
    if (expect('\'', E007_1)) {
        Token* token = makeToken(TOKEN_CHAR);
        token->value._char = c;

        return token;
    }

    return NULL;
}

static Token* string()
{
    advance();
    StringBuilder* builder = newSB();
    char c;

    while (!isAtEnd()) {
        c = peek();

        if (c == '"') {
            Token* token = makeToken(TOKEN_STRING);
            token->value.string = buildSB(builder);
            freeSB(builder);

            return token;
        }

        addSB(builder, c);
        advance();
    }

    addErrorAtCurrent(E008, peek());

    return NULL;
}

static Token* generateToken()
{
    char c = peek();

    if (isDigit(c)) {
        return number(false);
    }

    if (isAlpha(c)) {
        return identifier();
    }

    switch (c) {
        case '+': {
            if (match('=')) {
                return makeToken(TOKEN_PLUS_EQUAL);
            }

            return makeToken(match('+') ? TOKEN_DOUBLE_PLUS : TOKEN_PLUS);
        }
        case '-': {
            if (match('=')) {
                return makeToken(TOKEN_MINUS_EQUAL);
            }

            return makeToken(match('-') ? TOKEN_DOUBLE_MINUS : TOKEN_MINUS);
        }
        case '*': {
            if (match('*')) {
                return makeToken(match('=') ? TOKEN_DOUBLE_STAR_EQUAL : TOKEN_DOUBLE_STAR);
            }

            return makeToken(match('=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
        }
        case '/': return makeToken(match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
        case '%': return makeToken(match('=') ? TOKEN_MODULO_EQUAL : TOKEN_MODULO);
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case '[': return makeToken(TOKEN_LEFT_BRACKET);
        case ']': return makeToken(TOKEN_RIGHT_BRACKET);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case '=': return makeToken(match('=') ? TOKEN_DOUBLE_EQUAL : TOKEN_EQUAL);
        case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '.': return isDigit(peekNext()) ? number(true) : makeToken(TOKEN_DOT);
        case ',': return makeToken(TOKEN_COMMA);
        case '&': return expect('&', E009) ? makeToken(TOKEN_DOUBLE_AMPERSAND) : NULL;
        case '|': return expect('|', E005) ? makeToken(TOKEN_DOUBLE_PIPE) : NULL;
        case '?': return makeToken(TOKEN_QUESTION_MARK);
        case '#': return makeToken(TOKEN_HASHTAG);
        case ':': return makeToken(TOKEN_COLON);
        case '@': return makeToken(TOKEN_AT_SYMBOL);
        case '_': return makeToken(TOKEN_UNDERSCORE);
        case '\'': return char_();
        case '"': return string();
    }

    addErrorAtCurrent(E004, c);

    return NULL;
}

static void next()
{
    skipWhitespaces();
    scanner->startIndex = scanner->currentIndex;
    Token* token = generateToken();

    if (token != NULL) {
        vectorPush(scanner->tokens, token);
    }

    advance();
}

Vector* scan(Module* module)
{
    scanner = newScanner(module);

    while (!isAtEnd()) {
        next();
    }

    scanner->startIndex = scanner->currentIndex;
    Token* eofToken = makeToken(TOKEN_EOF);
    vectorPush(scanner->tokens, eofToken);

    Vector* tokens = scanner->tokens;
    freeScanner(scanner);

    return tokens;
}

void freeTokens(Vector* tokens)
{
    VEC_EACH(tokens) {
        Token* token = VEC_EACH_GET(tokens);
        free(token);
    }
    
    freeVector(tokens);
}
