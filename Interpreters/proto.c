#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/InterpretProgram.h"
#include "DaLanguageCBootstrap/DaLanguageList.h"
#include "DaLanguageCBootstrap/Grammar.h"

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

    //Ugly macro DSL file. Contains starter grammar in a `ListOfPointer` of
    //`Rule` called `grammar`. Non-terminal types have their names assigned in a
    //`ListOfPointer` of `const char*` called `names`. The `nonterminalType` of
    //each term is used to index this list. The node at the index's pointer
    //points to a C string containing the name.
    #include "DaLanguageCBootstrapConstants/protoGrammar.c"

    fputs("Starter grammar for proto is:\n", stderr);
    printGrammarWithNonterminalNames(grammar, stderr, names);

    SymbolTable protoSymbols = initializeSymbolTableSize(256);


    addInstructionSymbol(protoSymbols, "inputNext", input, (1, "value"), (2, "more", "end"));
    addInstructionSymbol(protoSymbols, "outputNext", output, (1, "value"), (2, "more", "end"));

    addInstructionSymbol(protoSymbols, "first", DaLanguageListFirst, (1, "list"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "last", DaLanguageListLast, (1, "list"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "next", DaLanguageListNext, (1, "list"), (2, "more", "end"));
    addInstructionSymbol(protoSymbols, "prev", DaLanguageListPrev, (1, "list"), (2, "more", "end"));

    addInstructionSymbolDefJump(protoSymbols, "copy", copy, (2, "from", "to"));


    addDataSymbol(protoSymbols, "input", inputChar);
    addDataSymbol(protoSymbols, "output", outputChar);
    addDataSymbol(protoSymbols, "grammar", grammar);


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
    finalizeGrammar(&grammar);
    finalizeListOfPointer(&names);

    return 0;

}
