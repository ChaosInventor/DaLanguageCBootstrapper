#ifndef DALCBOOT_INTERPRETER
#define DALCBOOT_INTERPRETER

#include "DaLanguageCBootstrap/Stack.h"
#include "DaLanguageCBootstrap/SemanticAnalyzer.h"
#include "DaLanguageCBootstrap/ListOfString.h"

#define DalInstruction(name) String name(SymbolTable params)
#define DalValue(name, type)\
    ConstString(name ## S, #name);\
    Symbol* name ## Sym = getSymbolTable(params, name ## S);\
    type* name = (type*)name ## Sym->Data.STData;
#define DalJump(name) ConstString(name, #name);

#define addDataSymbol(table, name, var)\
    ConstString(var ## S, name);\
    addSymbolTable(&(table), var ## S, createDataSymbol(&(var)));

struct InterpreterScope;

typedef enum InterpretationResultType
{

    IRunning,
    ISuccessful,
    IESemantic,

} InterpretationResultType;
typedef struct InterpretationResult
{

    InterpretationResultType Type;
    union
    {
        struct { String Jump; } ISuccessful;
        struct { SemanticAnalysisResult semanticsResult; } IESemantic;
    } Data;

} InterpretationResult;
typedef struct InterpreterScope
{

    SemanticDef* curDef;
    SemanticSpec* curSpec;
    Symbol* Next;
    String nextName;

} InterpreterScope;
typedef struct InterpreterContext
{

    SemanticDef* topDef;
    Stack Frame;
    InterpretationResult Result;

} InterpreterContext;

InterpreterScope* allocateInterpreterScope(void);

InterpreterScope initializeInterpreterScope(SemanticDef* def);
InterpreterContext initializeInterpreter(SymbolTable startingSymbols, SemanticDef* topDef);

InterpreterScope* createInterpreterScope(SemanticDef* def);

void runInterpreter(InterpreterContext* context);
bool bigStepInterpreter(InterpreterContext* context);
bool stepInterpreter(InterpreterContext* context);

void finalizeInterpreter(InterpreterContext* context);

void destroyInterpreterScope(InterpreterScope* scope);

#endif //DALCBOOT_INTERPRETER

