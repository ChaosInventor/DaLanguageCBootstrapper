#include "DaLanguageCBootstrap/SymbolTable.h"
#include "DaLanguageCBootstrap/Instruction.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

Symbol* allocateSymbol()
{

    Symbol* ret = malloc(sizeof(Symbol));
    assert(ret != NULL);

    *ret = (struct Symbol){0};

    return ret;

}
SymbolTable* allocateSymbolTable()
{

    SymbolTable* ret = malloc(sizeof(SymbolTable));
    assert(ret != NULL);

    *ret = (struct SymbolTable){0};

    return ret;

}

SymbolTable initializeSymbolTableSize(size_t size)
{

    SymbolTable ret = {0};

    ret.Table = createHashTableSize(size);

    return ret;

}
Symbol initializeSymbol(SymbolType type, SymbolStatus status, SymbolData data)
{

    Symbol ret = {
        .Type = type,
        .Status = status,
        .Data = data
    };

    return ret;

}

Symbol* createSymbol(SymbolType type, SymbolStatus status, SymbolData data)
{

    Symbol* ret = allocateSymbol();
    *ret = initializeSymbol(type, status, data);
    return ret;

}
Symbol* createDataSymbol(void* data)
{

    Symbol* ret = allocateSymbol();

    SymbolData da = {.STData = data};
    *ret = initializeSymbol(STData, STResolved, da);

    return ret;

}

Symbol cloneSymbol(Symbol symbol)
{
    Symbol ret = {.Type = symbol.Type, .Status = symbol.Status, .Data = symbol.Data};
    return ret;
}
SymbolTable cloneSymbolTable(SymbolTable table)
{

    SymbolTable ret = {0};

    ret.symbolsList = copyListOfString(table.symbolsList);
    ret.Table = cloneHashTable(table.Table);
    ret.namelessValues = copyListOfString(table.namelessValues);

    return ret;

}

Symbol* copySymbol(Symbol symbol)
{

    Symbol* ret = allocateSymbol();
    *ret = cloneSymbol(symbol);

    return ret;

}
SymbolTable* copySymbolTable(SymbolTable table)
{

    SymbolTable* ret = allocateSymbolTable();
    *ret = cloneSymbolTable(table);

    return ret;

}

Symbol* addSymbolTable(SymbolTable* table, String symName, Symbol* sym)
{

    assert(table != NULL);
    assert(!stringIsEqualToString(symName, EMPTY_STRING));

    Symbol* oldSym = (Symbol*)addHashTable(table->Table, symName, sym);
    if(oldSym == NULL)
    {
        appendStringToListOfString(cloneString(symName), &table->symbolsList);
    }

    return oldSym;

}
void addSymbolTableNameless(SymbolTable* table, Symbol* sym)
{

    appendStringToListOfString(cloneString(EMPTY_STRING), &table->symbolsList);
    appendPointerToListOfPointer(sym, &table->namelessValues);

}

Symbol* getSymbolTable(SymbolTable table, String key)
{

    Symbol** sym = (Symbol**)getHashTableKey(table.Table, key);

    if(sym == NULL) return NULL;
    else return *sym;

}

Symbol* resolveNamelessSymbol(SymbolTable* table, String name)
{

    assert(table != NULL);
    assert(!stringIsEqualToString(name, EMPTY_STRING));

    ListOfPointer* namelessValue = (ListOfPointer*)listPopFrontNode(&table->namelessValues);

    Symbol* sym = namelessValue->Pointer;

    free(namelessValue);

    ListOfString* nameNode = NULL;
    iterateListOfStringForward(table->symbolsList, curNode)
    {
        if(stringIsEqualToString(curNode->Name, EMPTY_STRING))
        {
            nameNode = curNode;
            break;
        }
    }
    if(nameNode == NULL)
    {
        return sym;
    }

    Symbol** oldSym = addHashTable(table->Table, name, sym);
    if(oldSym == NULL)
    {
        nameNode->Name = cloneString(name);
        return NULL;
    }
    else
    {
        listExtractNode(&nameNode->Header, &table->symbolsList);

        finalizeString(&nameNode->Name);
        free(nameNode);

        return *oldSym;
    }

}

char* symbolTypeToCString(SymbolType type)
{

    switch(type)
    {
        case STUnknown: return "STUnknown";
        case STDef: return "STDef";
        case STSpec: return "STSpec";
        case STJump: return "STJump";
        case STInstruction: return "STInstruction";
        case STData: return "STData";
        default: return "Not Known";
    }

}
char* symbolStatusToCString(SymbolStatus status)
{

    switch(status)
    {
        case STError: return "Error";
        case STUnresolved: return "Unresolved";
        case STResolved: return "Resolved";
        default: return "Unknown";
    }

}

void dumpSymbolTable(SymbolTable table, FILE* stream)
{

    fprintf(
            stream,
            "Symbol table has %zd named symbols and %zd unnamed symbols, which "
            "are:\n", table.symbolsList.Count, table.namelessValues.Count
           );

    fputs("\tNamed symbols:\n", stream);
    iterateListOfStringForward(table.symbolsList, curNode)
    {
        fputs("\t\t`", stream);
        writeString(curNode->Name, stream);
        fputs("`: ", stream);

        Symbol* sym = getSymbolTable(table, curNode->Name);
        if(sym != NULL) dumpSymbol(*sym, stream);
        else fputs("NULL", stream);
        fputs("\n", stream);
    }

    fputs("\tUnnamed symbols:\n", stream);
    iterateListOfPointerForward(table.namelessValues, curNode)
    {
        fputs("\t\t", stream);
        dumpSymbol(*(Symbol*)curNode->Pointer, stream);
        fputs("\n", stream);
    }

}
void dumpSymbol(Symbol sym, FILE* stream)
{

    fprintf(
            stream, "Symbol with type `%s`, status `%s` and data: ",
            symbolTypeToCString(sym.Type), symbolStatusToCString(sym.Status)
           );

    switch(sym.Type)
    {
        case STUnknown: fputs("<UNKNOWN>\n", stream); break;
        case STDef: fprintf(stream, "semantic def at %p\n", (void*)sym.Data.STDef); break;
        case STSpec: fprintf(stream, "semantic spec at %p\n", (void*)sym.Data.STSpec); break;
        case STJump:
                     {
                         fprintf(stream, "jump belonging to semantic def at %p with name `", (void*)sym.Data.STJump.Def);
                         writeString(sym.Data.STJump.Name, stream);
                         fputs("`\n", stream);
                     } break;
        case STInstruction: dumpInstruction(*sym.Data.STInstruction, stream); break;
        case STData: fprintf(stream, "data at %p\n", (void*)sym.Data.STData); break;
        case STReference: fprintf(stream, "reference to symbol at %p\n", (void*)sym.Data.STReference); break;

        default: fputs("<ENUM ERROR>\n", stream); break;
    }

}

void finalizeSymbolTable(SymbolTable* table, void (*cleanSymbol)(void*))
{

    assert(table != NULL);

    finalizeHashTable(&table->Table, cleanSymbol);
    finalizeListOfString(&table->symbolsList);
    finalizeListOfPointer(&table->namelessValues);

    *table = (struct SymbolTable){0};

}
void finalizeSymbol(Symbol* sym)
{

    assert(sym != NULL);

    switch(sym->Type)
    {
        case STDef: destroySemanticDef(sym->Data.STDef); break;
        case STSpec: destroySemanticSpec(sym->Data.STSpec); break;
        case STJump: finalizeString(&sym->Data.STJump.Name); break;
        case STInstruction: destroyInstruction(sym->Data.STInstruction); break;
        default: break;
    }

    *sym = (struct Symbol){0};

}

void destroySymbolTable(SymbolTable* table, void (*cleanSymbol)(void*))
{
    finalizeSymbolTable(table, cleanSymbol);
    free(table);
}
void destroySymbol(Symbol* sym)
{
    finalizeSymbol(sym);
    free(sym);
}

