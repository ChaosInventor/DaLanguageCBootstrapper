#ifndef DALCBOOT_SEMANTIC_ANALYZER
#define DALCBOOT_SEMANTIC_ANALYZER

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include "DaLanguageCBootstrap/SymbolTable.h"
#include "DaLanguageCBootstrap/Stack.h"
#include "DaLanguageCBootstrap/AST.h"

//TODO: Needs to be refactored, both interface and implementation

struct SemanticDef;
struct SemanticSpec;

typedef struct SemanticDef
{

    //Pointers to the symbol table's symbols list
    ListOfString* valuesEnd;
    ListOfString* jumpsEnd;
    ListOfString* defsEnd;
    ListOfString* specsEnd;
    SymbolTable Symbols;
    List Specs; //ListOfPointer of SemanticSpec*
    struct SemanticSpec* executionStart;
    size_t Line;
    size_t Column;

} SemanticDef;
typedef struct SemanticSpec
{

    SemanticDef* containingDef;
    Symbol* Referent;
    SymbolTable Values;
    SymbolTable Jumps;
    Symbol* Executable;
    SymbolTable executableValues;
    SymbolTable executableJumps;
    size_t Line;
    size_t Column;

} SemanticSpec;

typedef struct ParameterValue
{

    String Value;
    Symbol* symbolValue;
    
} ParameterValue;
typedef enum SemanticAnalysisErrorType
{

    SAEDuplicateSymbol,
    SAENoReferent,
    SAEMissingSymbol,
    SAEMultipleParameterValues,
    SAEReferentCycle,
    SAEIncorrectSymbolType,
    SAEUnknownParameter,
    SAETooManyParameters,

} SemanticAnalysisErrorType;
typedef enum SemanticAnalysisErrorLocation
{

    SASpec,
    SADef,

} SemanticAnalysisErrorLocation;
typedef enum SemanticListType
{

    SAValueList,
    SAJumpList,

} SemanticListType;
typedef struct SemanticAnalysisError
{
    
    SemanticAnalysisErrorType Type;
    SemanticAnalysisErrorLocation Location;
    size_t Line;
    size_t Column;
    union
    {
        struct
        {
            String duplicateName;
            SymbolType oldType;
            SymbolType newType;
        } SAEDuplicateSymbol;
        struct { String Name; } SAENoReferent;
        struct
        {
            String Name;
            SemanticListType Location;
        } SAEMissingSymbol;
        struct
        {
            String Parameter;
            List Values; //ListOfPointer of ParameterValue
            SemanticListType Type;
        } SAEMultipleParameterValues;
        struct { SemanticSpec** Specs; size_t Count; } SAEReferentCycle;
        struct
        {
            SymbolType Given;
            size_t acceptableSize;
            SymbolType* acceptableTypes;
        } SAEIncorrectSymbolType;
        struct { String Name; SemanticListType Location; } SAEUnknownParameter;
        struct { SemanticListType Location; } SAETooManyParameters;
    } Data;

} SemanticAnalysisError;
typedef struct SemanticAnalysisResult
{

    List Errors; //ListOfPointer Of SemanticAnalysisError

} SemanticAnalysisResult;

SemanticAnalysisError* allocateSemanticAnalysisError();
ParameterValue* allocateParameterValue();


SemanticAnalysisResult initializeSemanticDefFromAbstractDefAt(
    SemanticDef* ret, AbstractDef def
);
SemanticAnalysisResult initializeSemanticSpecFromAbstractSpecAndContainingDefAt(
    SemanticSpec* ret, AbstractSpec spec, SemanticDef* containingDef
);


SemanticDef cloneSemanticDef(SemanticDef def);


SemanticDef* createSemanticDefFromAbstractDef(
    AbstractDef def, SemanticAnalysisResult* result
);
SemanticSpec* createSemanticSpecFromAbstractSpecAndContainingDef(
    AbstractSpec spec, SemanticDef* containingDef,
    SemanticAnalysisResult* result
);

SemanticAnalysisError* createDuplicateErrorDef(
    SemanticDef def, String name, SymbolType old, SymbolType new
);
SemanticAnalysisError* createDuplicateErrorSpec(
    SemanticSpec spec, String name, SymbolType old, SymbolType new
);

SemanticAnalysisError* createNoReferentError(String name, SemanticSpec spec);

SemanticAnalysisError* createMissingSymbolErrorSpec(
    String name, SemanticListType location, SemanticSpec spec
);
SemanticAnalysisError* createMissingSymbolErrorDef(
    String name, SemanticListType location, SemanticDef def
);

SemanticAnalysisError* createMultipleParameterValuesError(
    String parameter, SemanticListType type, SemanticSpec spec
);

SemanticAnalysisError* createReferentCycleError(
    SemanticSpec** specs, size_t count, SemanticDef def
);

SemanticAnalysisError* createIncorrectSymbolTypeErrorSpec_v(
    SymbolType given, SemanticSpec spec, size_t acceptableSize, va_list args
);
SemanticAnalysisError* createIncorrectSymbolTypeErrorSpec(
    SymbolType given, SemanticSpec spec, size_t acceptableSize, ...
);
SemanticAnalysisError* createIncorrectSymbolTypeErrorDef_v(
    SymbolType given, SemanticDef def, size_t acceptableSize, va_list args
);
SemanticAnalysisError* createIncorrectSymbolTypeErrorDef(
    SymbolType given, SemanticDef def, size_t acceptableSize, ...
);

SemanticAnalysisError* createUnknownParameterErrorSpec(
    String name, SemanticListType location, SemanticSpec spec
);
SemanticAnalysisError* createUnknownParameterErrorDef(
    String name, SemanticListType location, SemanticDef def
);

SemanticAnalysisError* createTooManyParametersError(
    SemanticListType type, SemanticSpec spec
);


void appendStringToSemanticAnalysisError(
    String value, SemanticAnalysisError* error
);
void appendSymbolToSemanticAnalysisError(
    Symbol* symbol, SemanticAnalysisError* error
);
void appendStringAndSymbolToSemanticAnalysisError(
    String value, Symbol* symbol, SemanticAnalysisError* error
);

void appendValueToMultipleValuesError(
    SemanticAnalysisResult* results, String parameter, SemanticListType type,
    String symbolName, Symbol* symbol, SemanticSpec spec
);


SemanticAnalysisResult resolveSemanticDefSymbols(
    SemanticDef* def, SymbolTable symbols
);
SemanticAnalysisResult resolveSemanticSpecSymbols(
    SemanticSpec* spec, List* executableValues, List* executableJumps
);


SemanticAnalysisResult mergeSpecSymbolTables(
    SymbolTable from, SymbolTable* to, List* params, SemanticSpec spec,
    SemanticListType type
);


void dumpSemanticDef(SemanticDef def, FILE* stream);
void dumpSemanticSpec(SemanticSpec spec, FILE* stream);

void printSemanticAnalysisResult(SemanticAnalysisResult result, FILE* stream);


const char* semanticAnalysisErrorLocationToCString(
    SemanticAnalysisErrorLocation location
);
const char* semanticListTypeToCString(SemanticListType type);


void finalizeSemanticDef(SemanticDef* def);
void finalizeSemanticSpec(SemanticSpec* spec);
void finalizeSemanticAnalysisResult(SemanticAnalysisResult* result);
void finalizeSemanticAnalysisError(SemanticAnalysisError* error);


void destroySemanticDef(SemanticDef* def);
void destroySemanticSpec(SemanticSpec* spec);
void destroySemanticAnalysisResult(SemanticAnalysisResult* result);
void destroySemanticAnalysisError(SemanticAnalysisError* error);

#endif //DALCBOOT_SEMANTIC_ANALYZER

