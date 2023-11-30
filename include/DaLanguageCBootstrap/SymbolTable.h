#ifndef DALCBOOT_SYMBOL_TABLE
#define DALCBOOT_SYMBOL_TABLE

#include <stdio.h>
#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/HashTable.h"
#include "DaLanguageCBootstrap/String.h"
#include "DaLanguageCBootstrap/ListOfString.h"
#include "DaLanguageCBootstrap/ListOfPointer.h"
#include "DaLanguageCBootstrap/ListOfPointer.h"

#define addDataSymbol(table, name, var)\
    ConstString(var ## S, name);\
    addSymbolTable(&(table), var ## S, createDataSymbol(&(var)));

#define iterateSymbolTableForward(table, sym)\
    iterateListOfStringForward((table).symbolsList, curNode)\
        for(Symbol* sym = *getHashTableKey((table).Table, curNode->Name), *runOnce = (Symbol*)0x1; runOnce != NULL; runOnce = NULL)
#define iterateSymbolTableBackward(table, sym)\
    iterateListOfStringBackward((table).symbolsList, curNode)\
        for(Symbol* sym = *getHashTableKey((table).Table, curNode->Name), *runOnce = (Symbol*)0x1; runOnce != NULL; runOnce = NULL)

struct SemanticDef;
struct SemanticSpec;
struct Instruction;
struct Symbol;

void destroySemanticDef(struct SemanticDef* def);
void destroySemanticSpec(struct SemanticSpec* spec);
void destroyInstruction(struct Instruction* inst);

typedef struct SymbolTable
{

    List symbolsList; //ListOfString
    HashTable Table; //Symbol*
    List namelessValues; //ListOfPointer of Symbol*

} SymbolTable;

typedef enum SymbolType
{

    STUnknown,
    STDef,
    STSpec,
    STJump,
    STInstruction,
    STData,
    STReference,

} SymbolType;
typedef enum SymbolStatus { STError, STUnresolved, STResolved } SymbolStatus;
typedef union SymbolData
{

    struct SemanticDef* STDef;
    struct SemanticSpec* STSpec;
    struct
    {
        struct SemanticDef* Def;
        String Name;
    } STJump;
    struct Instruction* STInstruction;
    void* STData;
    struct Symbol* STReference;

} SymbolData;
typedef struct Symbol
{

    SymbolType Type;
    SymbolStatus Status;
    SymbolData Data;

} Symbol;

//FIXME: Required for automatic object dependencies, however it is a circular
//inclusion. The function `dumpInstruction` is required by `dumpSymbol`.
#include "DaLanguageCBootstrap/Instruction.h"

Symbol* allocateSymbol(void);
SymbolTable* allocateSymbolTable(void);

SymbolTable initializeSymbolTableSize(size_t size);
Symbol initializeSymbol(SymbolType type, SymbolStatus status, SymbolData data);

Symbol* createSymbol(SymbolType type, SymbolStatus status, SymbolData data);
Symbol* createDataSymbol(void* data);

Symbol cloneSymbol(Symbol symbol);
SymbolTable cloneSymbolTable(SymbolTable table);

Symbol* copySymbol(Symbol symbol);
SymbolTable* copySymbolTable(SymbolTable table);

Symbol* addSymbolTable(SymbolTable* table, String symName, Symbol* sym);
void addSymbolTableNameless(SymbolTable* table, Symbol* sym);

Symbol* getSymbolTable(SymbolTable table, String key);

Symbol* resolveNamelessSymbol(SymbolTable* table, String name);

char* symbolTypeToCString(SymbolType type);
char* symbolStatusToCString(SymbolStatus status);

void dumpSymbolTable(SymbolTable table, FILE* stream);
void dumpSymbol(Symbol sym, FILE* stream);

void finalizeSymbolTable(SymbolTable* table, void (*cleanSymbol)(void*));
void finalizeSymbol(Symbol* sym);

void destroySymbolTable(SymbolTable* table, void (*cleanSymbol)(void*));
void destroySymbol(Symbol* sym);

#endif //DALCBOOT_SYMBOL_TABLE

