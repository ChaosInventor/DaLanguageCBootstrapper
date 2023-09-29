#ifndef INTERPRET_PROGRAM
#define INTERPRET_PROGRAM

#include <stdio.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Parser.h"
#include "DaLanguageCBootstrap/AST.h"
#include "DaLanguageCBootstrap/SemanticAnalyzer.h"
#include "DaLanguageCBootstrap/Interpreter.h"
#include "DaLanguageCBootstrap/SymbolTable.h"

typedef enum ProgramResultType
{

    PRSuccessful,
    PREFile,
    PREParser,
    PRENoDef,
    PRESemantic,
    PREInterpretation,

} ProgramResultType;
typedef struct ProgramResult
{

    ProgramResultType Type;
    union
    {
        struct { String jumpResult; } PRSuccessful;
        struct { const char* Program; const char* Perms; } PREFile;
        struct { ParserContext Parser; Token errorToken; } PREParser;
        struct { ParserContext Parser; } PRENoDef;
        struct { SemanticAnalysisResult Result; } PRESemantic;
        struct { InterpreterContext Interpreter; } PREInterpretation;
    } Data;

} ProgramResult;

ProgramResult interpretProgram(const char* program, SymbolTable symbols);

void printProgramResult(
    ProgramResult result, FILE* stream, const char* interpreterName,
    const char* program
);

void finalizeProgramResult(ProgramResult* result);

void destroyProgramResult(ProgramResult* result);

#endif //INTERPRET_PROGRAM

