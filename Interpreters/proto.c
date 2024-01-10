#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/InterpretProgram.h"
#include "DaLanguageCBootstrap/DaLanguageList.h"
#include "DaLanguageCBootstrap/DaLanguageListOfPointer.h"
#include "DaLanguageCBootstrap/DaLanguageGrammar.h"
#include "DaLanguageCBootstrap/Grammar.h"

char inputChar;
char outputChar;
DaLanguageList grammarList;
DaLanguageList dataList;
DaLanguageList list;
ListNode* node;

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

    //Ugly macro DSL file. Contains starter grammar in a `ListOfPointer` of
    //`Rule` called `grammar`. Non-terminal types have their names assigned in a
    //`ListOfPointer` of `const char*` called `names`. The `nonterminalType` of
    //each term is used to index this list. The node at the index's pointer
    //points to a C string containing the name. The
    //`memoizeNonterminalTypeInListOfNamesWithName` function handles mapping
    //non-terminal types to C strings.
    #include "DaLanguageCBootstrapConstants/protoGrammar.c"

    fputs("Starter grammar for proto is:\n", stderr);
    printGrammarWithNonterminalNames(grammar, stderr, names);

    grammarList.List = grammar;

    ListNode* startRule = NULL;
    iterateListOfPointerForward(grammar, curNode)
    {
        Rule* rule = (Rule*)curNode->Pointer;
        iterateListOfPointerForward(rule->leftAlterations, curDefNode)
        {
            Definition* def = (Definition*)curDefNode->Pointer;

            Group* group = (Group*)((ListOfPointer*)def->Groups.First)->Pointer;
            if(group == NULL) continue;

            if(group->Type == GTerm)
            {
                if(group->Data.GTerm.Type == GNonterminal)
                {
                    if(
                       group->Data.GTerm.Data.GNonterminal.nonterminalType
                       ==
                       memoizeNonterminalTypeInListOfNamesWithName(&names, "Instruction definition")
                      )
                    {
                        startRule = &curNode->Header;
                    }
                }
            }
        }
    }
    assert(startRule != NULL);

    grammarList.curNode = startRule;

    SymbolTable protoSymbols = initializeSymbolTableSize(256);


    addInstructionSymbol(protoSymbols, "inputNext", input, (1, "value"), (2, "more", "end"));
    addInstructionSymbol(protoSymbols, "outputNext", output, (1, "value"), (2, "more", "end"));
    addInstructionSymbolDefJump(protoSymbols, "copyChar", copy, (2, "from", "to"));

    addInstructionSymbol(protoSymbols, "createList", DaLanguageListCreate, (1, "pointer"), (1, "next"));

    addInstructionSymbol(protoSymbols, "first", DaLanguageListFirst, (1, "dList"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "last", DaLanguageListLast, (1, "dList"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "next", DaLanguageListNext, (1, "dList"), (2, "more", "end"));
    addInstructionSymbol(protoSymbols, "prev", DaLanguageListPrev, (1, "dList"), (2, "more", "end"));

    addInstructionSymbol(protoSymbols, "suffix", DaLanguageListSuffix, (2, "dList", "node"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "prefix", DaLanguageListPrefix, (2, "dList", "node"), (2, "notNull", "null"));

    addInstructionSymbol(protoSymbols, "unsuffix", DaLanguageListUnsuffix, (2, "dList", "node"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "unprefix", DaLanguageListUnprefix, (2, "dList", "node"), (2, "notNull", "null"));
    addInstructionSymbol(protoSymbols, "extract", DaLanguageListExtract, (2, "dList", "node"), (2, "notNull", "null"));

    addInstructionSymbol(protoSymbols, "destroyList", DaLanguageListDestroy, (1, "pointer"), (1, "next"));

    addInstructionSymbol(protoSymbols, "createPointer", DaLanguageListOfPointerCreate, (1, "node"), (1, "next"));

    addInstructionSymbol(protoSymbols, "copyPointer", DaLanguageListOfPointerCopy, (2, "from", "to"), (1, "next"));

    addInstructionSymbol(protoSymbols, "nullPointer", DaLanguageListOfPointerIsNull, (1, "dList"), (2, "null", "notNull"));

    addInstructionSymbol(protoSymbols, "nestedListInsert", DaLanguageListOfPointerNestedListInsert, (2, "nested", "dList"), (1, "next"));
    addInstructionSymbol(protoSymbols, "nestedListExtract", DaLanguageListOfPointerNestedListExtract, (2, "nested", "dList"), (1, "next"));

    addInstructionSymbol(protoSymbols, "destroyPointer", DaLanguageListOfPointerDestroy, (1, "node"), (1, "next"));

    addInstructionSymbol(protoSymbols, "ruleLeft", DaLanguageGrammarRuleLeft, (2, "rules", "dList"), (1, "next"));
    addInstructionSymbol(protoSymbols, "ruleRight", DaLanguageGrammarRuleRight, (2, "rules", "dList"), (1, "next"));

    addInstructionSymbol(protoSymbols, "definitionGroups", DaLanguageGrammarDefinitionGroups, (2, "definitions", "dList"), (1, "next"));

    addInstructionSymbol(protoSymbols, "groupType", DaLanguageGrammarGroupType, (1, "dList"), (2, "term", "definition"));
    addInstructionSymbol(protoSymbols, "groupDefinitions", DaLanguageGrammarGroupDefinitions, (2, "groups", "dList"), (1, "next"));

    addInstructionSymbol(protoSymbols, "termType", DaLanguageGrammarTermType, (1, "dList"), (2, "terminal", "nonterminal"));
    addInstructionSymbol(protoSymbols, "printTerm", DaLanguageGrammarPrintTerm, (1, "dList"), (1, "next"));

    addInstructionSymbolDefJump(protoSymbols, "brk", brk, (0));


    addDataSymbol(protoSymbols, "input", inputChar);
    addDataSymbol(protoSymbols, "output", outputChar);
    addDataSymbol(protoSymbols, "grammar", grammarList);
    addDataSymbol(protoSymbols, "data", dataList);
    addDataSymbol(protoSymbols, "node", node);
    addDataSymbol(protoSymbols, "list", list);


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
