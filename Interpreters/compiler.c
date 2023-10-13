#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/InterpretProgram.h"

const char* stdoutFD = "1";
const char* writeSyscall = "4";
const char* exitSyscall = "1";
const char* msg = "msg";
const char* msgLen = "len";
const char* zerox80 = "0x80";
const char* zerox0 = "0x0";
const char* eax = "eax";
const char* ebx = "ebx";
const char* ecx = "ecx";
const char* edx = "edx";

DalInstruction(compilerMov)
{

    DalValue(l, const char*);
    DalValue(r, const char*);
    DalJump(next);

    printf("\tmov %s, %s\n", *l, *r);

    return next;

}
DalInstruction(compilerInt)
{

    DalValue(num, const char*);
    DalJump(next);

    printf("\tint %s\n", *num);

    return next;

}

static void destroySymbolVoid(void* sym)
{
    if(sym == NULL) return;
    destroySymbol((Symbol*)sym);
}
int main(int argc, char* argv[])
{

    SymbolTable compilerSymbols = initializeSymbolTableSize(256);

    addInstructionSymbolDefJump(compilerSymbols, "mov", compilerMov, (2, "l", "r"));
    addInstructionSymbolDefJump(compilerSymbols, "int",  compilerInt, (1, "num"));

    addDataSymbol(compilerSymbols, "stdoutFD", stdoutFD);
    addDataSymbol(compilerSymbols, "writeSyscall", writeSyscall);
    addDataSymbol(compilerSymbols, "exitSyscall", exitSyscall);
    addDataSymbol(compilerSymbols, "msg", msg);
    addDataSymbol(compilerSymbols, "msgLen", msgLen);
    addDataSymbol(compilerSymbols, "0x80", zerox80);
    addDataSymbol(compilerSymbols, "0x0", zerox0);
    addDataSymbol(compilerSymbols, "eax", eax);
    addDataSymbol(compilerSymbols, "ebx", ebx);
    addDataSymbol(compilerSymbols, "ecx", ecx);
    addDataSymbol(compilerSymbols, "edx", edx);

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

        printf(
                "global _start\n"
                "\n"
                "section .data\n"
                "\tmsg db \"Hello, world!\", 0x0a\n"
                "\tlen equ $ - msg\n"
                "\n"
                "section .text\n"
                "_start:\n"
              );

        ProgramResult result = interpretProgram(argv[i], compilerSymbols);

        fprintf(stderr, "%s: Finished interpreting %s, printing results\n", argv[0], argv[i]);
        printProgramResult(result, stderr, argv[0], argv[i]);
        finalizeProgramResult(&result);
    }

    finalizeSymbolTable(&compilerSymbols, destroySymbolVoid);

    return 0;

}

