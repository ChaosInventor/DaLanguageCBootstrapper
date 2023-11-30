#ifndef DALCBOOT_INSTRUCTION
#define DALCBOOT_INSTRUCTION

#include <stdio.h>
#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/String.h"
#include "DaLanguageCBootstrap/ListOfString.h"
#include "DaLanguageCBootstrap/SymbolTable.h"

#define DalInstruction(name) String name(SymbolTable params)
#define DalValue(name, type)\
    ConstString(name ## S, #name);\
    Symbol* name ## Sym = getSymbolTable(params, name ## S);\
    type* name = (type*)name ## Sym->Data.STData;
#define DalJump(name) ConstString(name, #name);

#define addInstructionSymbol(table, name, function, values, jumps)\
    ConstString(function ## S, name);\
    addSymbolTable(\
            &(table), function ## S,\
            createInstructionSymbol(createInstruction(\
                    function, generateListOfString values,\
                    generateListOfString jumps)\
                    )\
            );
#define addInstructionSymbolDefJump(table, name, function, values)\
    addInstructionSymbol(table, name, function, values, (1, "next"))

typedef String (*InstructionFunc)(SymbolTable params);
typedef struct Instruction
{

    InstructionFunc Execute;
    List Values; //ListOfString
    List Jumps; //ListOfString

} Instruction;

Instruction* allocateInstruction(void);

Instruction initializeInstruction(InstructionFunc func, List values, List jumps);
Instruction initializeInstructionNoJump(InstructionFunc func, List values);

Instruction* createInstruction(InstructionFunc func, List values, List jumps);
Instruction* createInstructionNoJump(InstructionFunc func, List values);
Symbol* createInstructionSymbol(Instruction* inst);

void dumpInstruction(Instruction inst, FILE* stream);

void finalizeInstruction(Instruction* inst);

void destroyInstruction(Instruction* inst);

#endif //DALCBOOT_INSTRUCTION

