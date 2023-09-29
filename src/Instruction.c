#include "DaLanguageCBootstrap/Instruction.h"
#include <stdlib.h>
#include <assert.h>

Instruction* allocateInstruction()
{

    Instruction* ret = malloc(sizeof(Instruction));
    assert(ret != NULL);

    *ret = (struct Instruction){0};

    return ret;

}

Instruction initializeInstruction(InstructionFunc func, List values, List jumps)
{

    Instruction ret = {0};

    ret.Execute = func;
    ret.Values = values;
    ret.Jumps = jumps;

    return ret;

}
Instruction initializeInstructionNoJump(InstructionFunc func, List values)
{

    ConstString(next, "next");

    List defaultJumps = {0};
    appendStringToListOfString(next, &defaultJumps);

    return initializeInstruction(func, values, defaultJumps);

}

Instruction* createInstruction(InstructionFunc func, List values, List jumps)
{

    Instruction* ret = allocateInstruction();

    *ret = initializeInstruction(func, values, jumps);

    return ret;

}
Instruction* createInstructionNoJump(InstructionFunc func, List values)
{

    Instruction* ret = allocateInstruction();

    *ret = initializeInstructionNoJump(func, values);

    return ret;

}
Symbol* createInstructionSymbol(Instruction* inst)
{

    Symbol* ret = allocateSymbol();

    SymbolData data = {.STInstruction = inst};
    *ret = initializeSymbol(STInstruction, STResolved, data);

    return ret;

}

void dumpInstruction(Instruction inst, FILE* stream)
{

    //FIXME: Not standard complaint, cannot cast func pointer to void* on all
    //platforms
    fprintf(stream, "%p(", (void*)inst.Execute);

    iterateListOfStringForward(inst.Values, curNode)
    {
        writeString(curNode->Name, stream);
        if(curNode->Header.Next != NULL) fputc(',', stream);
    }
    fputs(")[", stream);
    iterateListOfStringForward(inst.Jumps, curNode)
    {
        writeString(curNode->Name, stream);
        if(curNode->Header.Next != NULL) fputc(',', stream);
    }
    fputs("]\n", stream);

}

void finalizeInstruction(Instruction* inst)
{

    assert(inst != NULL);

    finalizeListOfString(&inst->Values);
    finalizeListOfString(&inst->Jumps);

    *inst = (struct Instruction){0};

}

void destroyInstruction(Instruction* inst)
{

    finalizeInstruction(inst);
    free(inst);

}

