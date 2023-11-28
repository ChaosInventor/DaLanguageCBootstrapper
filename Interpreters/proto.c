#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/InterpretProgram.h"

char inputChar;
char outputChar;

DalInstruction(input)
{

    DalValue(value, char);
    DalJump(more);
    DalJump(end);

    int result = getchar();

    if(result == EOF)
    {
        return end;
    }
    else
    {
        *value = result;
        return more;
    }

}
DalInstruction(output)
{

    DalValue(value, char);
    DalJump(more);
    DalJump(end);

    int result = putchar(*value);

    if(result == EOF)
    {
        return end;
    }
    else
    {
        return more;
    }

}
DalInstruction(copy)
{

    DalValue(from, char);
    DalValue(to, char);
    DalJump(next);

    *to = *from;

    return next;

}

static void destroySymbolVoid(void* sym)
{
    if(sym == NULL) return;
    destroySymbol((Symbol*)sym);
}
int main(int argc, char* argv[])
{

    SymbolTable protoSymbols = initializeSymbolTableSize(256);

    addInstructionSymbol(protoSymbols, "inputNext", input, (1, "value"), (2, "more", "end"));
    addInstructionSymbol(protoSymbols, "outputNext", output, (1, "value"), (2, "more", "end"));

    addInstructionSymbolDefJump(protoSymbols, "copy", copy, (2, "from", "to"));

    addDataSymbol(protoSymbols, "input", inputChar);
    addDataSymbol(protoSymbols, "output", outputChar);

    assert(argc > 0);
    if(argc < 2)
    {
        fprintf(stderr, "%s: No program given\n", argv[0]);
        fprintf(stderr,
                "%s: Usage: %s <INSTRUCTION SYSTEM PROGRAM NAMES>\n", argv[0],
                argv[0]
               );
    }

    for(int i = 1; i < argc; ++i)
    {
        fprintf(stderr, "%s: Interpreting %s...\n", argv[0], argv[i]);

        ProgramResult result = interpretProgram(argv[i], protoSymbols);

        fprintf(stderr, "%s: Finished interpreting %s, printing results\n", argv[0], argv[i]);
        printProgramResult(result, stderr, argv[0], argv[i]);
        finalizeProgramResult(&result);
    }

    finalizeSymbolTable(&protoSymbols, destroySymbolVoid);

    return 0;

}
