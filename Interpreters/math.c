#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/InterpretProgram.h"

long long num1 = 0;
long long num2 = 0;
long long num3 = 0;
long long num4 = 0;
long long zero = 0;
long long one = 1;

DalInstruction(mathRead)
{

    DalValue(num, long long);
    DalJump(next);

    scanf("%lli", num);
    fprintf(stderr, "Read a number = %lli\n", *num);

    return next;

}
DalInstruction(mathWrite)
{

    DalValue(num, long long);
    DalJump(next);

    fprintf(stderr, "Printing value of number at %p\n", (void*)num);

    printf("%lli\n", *num);

    return next;

}
DalInstruction(mathAdd)
{

    DalValue(num1, long long);
    DalValue(num2, long long);
    DalValue(dest, long long);
    DalJump(next);

    fprintf(stderr, "Adding %lli with %lli to get %lli\n", *num1, *num2, *num1 + *num2);

    *dest = *num1 + *num2;

    return next;

}
DalInstruction(mathSubtract)
{

    DalValue(num1, long long);
    DalValue(num2, long long);
    DalValue(dest, long long);
    DalJump(next);

    fprintf(stderr, "Subtracting %lli with %lli to get %lli\n", *num1, *num2, *num1 - *num2);

    *dest = *num1 - *num2;

    return next;

}
DalInstruction(mathMultiply)
{

    DalValue(num1, long long);
    DalValue(num2, long long);
    DalValue(dest, long long);
    DalJump(next);

    fprintf(stderr, "Multiplying %lli with %lli to get %lli\n", *num1, *num2, *num1 * *num2);

    *dest = *num1 * *num2;

    return next;

}
DalInstruction(mathDivide)
{

    DalValue(num1, long long);
    DalValue(num2, long long);
    DalValue(dest, long long);
    DalJump(next);

    if(*num2 == 0)
    {
        printf("Error, cannot dive %lli by 0, aborting...\n", *num1);
        abort();
    }

    fprintf(stderr, "Dividing %lli with %lli to get %lli\n", *num1, *num2, *num1 / *num2);

    *dest = *num1 / *num2;

    return next;

}
DalInstruction(mathGreater)
{

    DalValue(num1, long long);
    DalValue(num2, long long);
    ConstString(trueRet, "true");
    ConstString(falseRet, "false");

    fprintf(stderr, "Comparing the value of %lli with %lli\n", *num1, *num2);
    if(*num1 > *num2)
    {
        return trueRet;
    }
    else
    {
        return falseRet;
    }

}

DalInstruction(brk)
{

    (void)params;
    DalJump(next);

    raise(SIGTRAP);

    return next;

}

static void destroySymbolVoid(void* sym)
{
    if(sym == NULL) return;
    destroySymbol((Symbol*)sym);
}
int main(int argc, char* argv[])
{

    SymbolTable mathSymbols = initializeSymbolTableSize(256);

    addInstructionSymbolDefJump(mathSymbols, "read", mathRead, (1, "num"));
    addInstructionSymbolDefJump(mathSymbols, "write", mathWrite, (1, "num"));
    addInstructionSymbolDefJump(mathSymbols, "add", mathAdd, (3, "num1", "num2", "dest"));
    addInstructionSymbolDefJump(mathSymbols, "subtract", mathSubtract, (3, "num1", "num2", "dest"));
    addInstructionSymbolDefJump(mathSymbols, "multiply", mathMultiply, (3, "num1", "num2", "dest"));
    addInstructionSymbolDefJump(mathSymbols, "divide", mathDivide, (3, "num1", "num2", "dest"));
    addInstructionSymbol(mathSymbols, "greater", mathGreater, (2, "num1", "num2"), (2, "true", "false"));
    addInstructionSymbolDefJump(mathSymbols, "brk", brk, (0));

    addDataSymbol(mathSymbols, "num1", num1);
    addDataSymbol(mathSymbols, "num2", num2);
    addDataSymbol(mathSymbols, "num3", num3);
    addDataSymbol(mathSymbols, "num4", num4);
    addDataSymbol(mathSymbols, "zero", zero);
    addDataSymbol(mathSymbols, "one", one);

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
        ProgramResult result = interpretProgram(argv[i], mathSymbols);
        fprintf(stderr, "%s: Finished interpreting %s, printing results\n", argv[0], argv[i]);
        printProgramResult(result, stderr, argv[0], argv[i]);
        finalizeProgramResult(&result);
    }

    finalizeSymbolTable(&mathSymbols, destroySymbolVoid);

    return 0;

}

