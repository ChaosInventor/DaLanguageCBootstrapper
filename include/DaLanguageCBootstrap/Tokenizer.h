#ifndef DALCBOOT_TOKENIZER
#define DALCBOOT_TOKENIZER

#include <stdio.h>
#include <stdbool.h>
#include "DaLanguageCBootstrap/String.h"

typedef enum TokenType
{

    End,
    Ignorable,
    Name,
    LabelMarker,
    Separator,
    ValueListStart,
    ValueListEnd,
    JumpListStart,
    JumpListEnd,
    DefStart,
    DefEnd,
    SpecTerm,

} TokenType;
typedef struct Token
{

    TokenType Type;
    String Data;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;

} Token;

typedef struct TokenizerContext
{

    FILE* Stream;
    String Buffer;
    StringNode* curNode;
    size_t curIndex;
    char curChar;
    size_t curLine;
    size_t curColumn;

} TokenizerContext;

TokenizerContext initializeTokenizerSize(FILE* stream, size_t bufferSize);
TokenizerContext initializeTokenizer(FILE* stream);

Token nextToken(TokenizerContext* context);

const char* tokenTypeToCString(TokenType type);

bool isLetter(char c);
bool isIgnorable(char c);

void printToken(Token token, FILE* stream);

void finalizeToken(Token* token);
void finalizeTokenizer(TokenizerContext* context);

#endif //DALCBOOT_TOKENIZER

