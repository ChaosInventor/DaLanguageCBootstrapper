#include "DaLanguageCBootstrap/DaLanguageGrammar.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

DalInstruction(DaLanguageGrammarRuleLeft)
{

    DalValue(rules, DaLanguageList);
    DalValue(dList, DaLanguageList);
    DalJump(next);

    assert(rules->curNode != NULL);
    Rule* rule = (Rule*)((ListOfPointer*)rules->curNode)->Pointer;

    ListOfPointer* pointerNode = (ListOfPointer*)dList->curNode;
    assert(pointerNode != NULL);

    DaLanguageList* newList = malloc(sizeof(DaLanguageList));
    assert(newList != NULL);

    *newList = (struct DaLanguageList){ rule->leftAlterations, NULL };
    pointerNode->Pointer = newList;

    return next;

}
DalInstruction(DaLanguageGrammarRuleRight)
{

    DalValue(rules, DaLanguageList);
    DalValue(dList, DaLanguageList);
    DalJump(next);

    assert(rules->curNode != NULL);
    Rule* rule = (Rule*)((ListOfPointer*)rules->curNode)->Pointer;

    ListOfPointer* pointerNode = (ListOfPointer*)dList->curNode;
    assert(pointerNode != NULL);

    DaLanguageList* newList = malloc(sizeof(DaLanguageList));
    assert(newList != NULL);

    *newList = (struct DaLanguageList){ rule->rightAlterations, NULL };
    pointerNode->Pointer = newList;

    return next;

}

DalInstruction(DaLanguageGrammarDefinitionGroups)
{

    DalValue(definitions, DaLanguageList);
    DalValue(dList, DaLanguageList);
    DalJump(next);

    assert(definitions->curNode != NULL);
    Definition* def = (Definition*)((ListOfPointer*)definitions->curNode)->Pointer;

    DaLanguageList* newList = malloc(sizeof(DaLanguageList));
    assert(newList != NULL);

    *newList = (struct DaLanguageList){ def->Groups, NULL };

    assert(dList->curNode != NULL);
    ((ListOfPointer*)dList->curNode)->Pointer = newList;

    return next;

}

DalInstruction(DaLanguageGrammarGroupType)
{

    DalValue(dList, DaLanguageList);
    DalJump(term);
    DalJump(definition);

    assert(dList->curNode != NULL);

    Group* group = (Group*)((ListOfPointer*)dList->curNode)->Pointer;

    if(group->Type == GTerm) return term;
    else if(group->Type == GDefinition) return definition;
    else assert(false);

}
DalInstruction(DaLanguageGrammarGroupDefinitions)
{

    DalValue(groups, DaLanguageList);
    DalValue(dList, DaLanguageList);
    DalJump(next);

    assert(groups->curNode != NULL);
    Group* group = (Group*)((ListOfPointer*)groups->curNode)->Pointer;

    DaLanguageList* newList = malloc(sizeof(DaLanguageList));
    assert(newList != NULL);

    *newList = (struct DaLanguageList){ group->Data.GDefinition, NULL };

    assert(dList->curNode != NULL);
    ((ListOfPointer*)dList->curNode)->Pointer = newList;

    return next;

}

DalInstruction(DaLanguageGrammarTermType)
{

    DalValue(dList, DaLanguageList);
    DalJump(terminal);
    DalJump(nonterminal);

    ListOfPointer* pointerNode = (ListOfPointer*)dList->curNode;
    assert(pointerNode != NULL);

    Group* group = (Group*)pointerNode->Pointer;
    assert(group->Type == GTerm);

    if(group->Data.GTerm.Type == GTerminal) return terminal;
    else return nonterminal;

}

DalInstruction(DaLanguageGrammarPrintTerm)
{

    DalValue(dList, DaLanguageList);
    DalJump(next);

    ListOfPointer* pointerNode = (ListOfPointer*)dList->curNode;
    assert(pointerNode != NULL);

    Group* group = (Group*)pointerNode->Pointer;
    assert(group->Type == GTerm);
    Term term = group->Data.GTerm;
    assert(term.Type == GTerminal);

    fputc(term.Data.GTerminal, stdout);
    fputc('\n', stdout);

    return next;

}
