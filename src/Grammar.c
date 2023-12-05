#include "DaLanguageCBootstrap/Grammar.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

Rule* allocateRule()
{

    Rule* ret = malloc(sizeof(Rule));
    assert(ret != NULL);

    *ret = (struct Rule){0};

    return ret;

}

Definition* allocateDefinition()
{

    Definition* ret = malloc(sizeof(Definition));
    assert(ret != NULL);

    *ret = (struct Definition){0};

    return ret;

}

Group* allocateGroup()
{

    Group* ret = malloc(sizeof(Group));
    assert(ret != NULL);

    *ret = (struct Group){0};

    return ret;

}


Rule initializeRuleFromLeftAlterationsAndRightAlterations(List left, List right)
{

    Rule ret = {.leftAlterations = left, .rightAlterations = right};
    return ret;

}

Definition initializeDefinition()
{

    Definition ret = {.lowerRepetition = 1, .upperRepetition = 1};
    return ret;

}
Definition initializeDefinitionWithGroupsAndRepetition(
    List groups, size_t lower, size_t upper
)
{

    Definition ret = {
        .Groups = groups, .lowerRepetition = lower, .upperRepetition = upper
    };
    return ret;

}
Definition initializeDefinitionWithGroup(Group group)
{

    Definition ret = initializeDefinition();

    Group* p = allocateGroup();
    *p = group;

    appendPointerToListOfPointer(p, &ret.Groups);

    return ret;

}

Group initializeGroupFromTerm(Term term)
{

    Group ret = {.Type = GTerm, .Data.GTerm = term};
    return ret;

}
Group initializeGroupFromDefinitions(List definitons)
{

    Group ret = {.Type = GDefinition, .Data.GDefinition = definitons};
    return ret;

}

Term initializeTermFromTerminal(char terminal)
{

    Term ret = {.Type = GTerminal, .Data.GTerminal = terminal};
    return ret;

}
Term initializeTermFromNonterminalType(int type)
{

    Term ret = {.Type = GNonterminal, .Data.GNonterminal.nonterminalType = type};
    return ret;

}


Rule* createRuleFromLeftAlterationsAndRightAlterations(List left, List right)
{

    Rule* ret = allocateRule();
    *ret = initializeRuleFromLeftAlterationsAndRightAlterations(left, right);
    return ret;

}

Definition* createDefinitionWithGroup(Group group)
{

    Definition* ret = allocateDefinition();

    *ret = initializeDefinitionWithGroup(group);

    return ret;

}
Definition* createDefinitionWithGroupsAndRepetition(
    List groups, size_t lower, size_t upper
)
{

    Definition* ret = allocateDefinition();
    *ret = initializeDefinitionWithGroupsAndRepetition(groups, lower, upper);
    return ret;

}

Group* createGroupFromTerm(Term term)
{

    Group* ret = allocateGroup();

    *ret = initializeGroupFromTerm(term);

    return ret;

}
Group* createGroupFromDefinitions(List definitions)
{

    Group* ret = allocateGroup();
    *ret = initializeGroupFromDefinitions(definitions);
    return ret;

}


void appendLeftAlternativeToRule(Definition left, Rule* rule)
{

    assert(rule != NULL);

    Definition* def = allocateDefinition();
    *def = left;
    appendPointerToListOfPointer(def, &rule->leftAlterations);

}
void appendRightAlternativeToRule(Definition right, Rule* rule)
{

    assert(rule != NULL);

    Definition* def = allocateDefinition();
    *def = right;
    appendPointerToListOfPointer(def, &rule->rightAlterations);

}

void appendGroupToDefinition(Group group, Definition* definition)
{

    Group* p = allocateGroup();
    *p = group;
    appendPointerToListOfPointer(p, &definition->Groups);

}


int memoizeNonterminalTypeInListOfNamesWithName(List* names, const char* name)
{

    int curType = 0;
    iterateListOfPointerForward(*names, curName)
    {
        if(strcmp((const char*)curName->Pointer, name) == 0)
        {
            return curType;
        }

        ++curType;
    }

    appendPointerToListOfPointer((char*)name, names);

    return curType;

}



bool termIsEqualToTerm(Term t1, Term t2)
{

    if(t1.Type != t2.Type)
    {
        return false;
    }

    if(t1.Type == GTerminal) return t1.Data.GTerminal == t2.Data.GTerminal;
    else
        return
            t1.Data.GNonterminal.nonterminalType
            ==
            t2.Data.GNonterminal.nonterminalType;

}
bool termIsIdenticalToTerm(Term t1, Term t2)
{

    if(t1.Type != t2.Type) return false;

    if(t1.Type == GTerminal) return t1.Data.GTerminal == t2.Data.GTerminal;
    else
    {
        if(
            t1.Data.GNonterminal.nonterminalType
            !=
            t2.Data.GNonterminal.nonterminalType
          ) return false;

        //Aliases
        List t1Terms = t1.Data.GNonterminal.Terms;
        List t2Terms = t2.Data.GNonterminal.Terms;
        if(t1Terms.Count != t2Terms.Count) return false;

        ListOfPointer* curNode2 = FirstListOfPointer(t2Terms);
        iterateListOfPointerForward(t1Terms, curNode1)
        {
            Term subTerm1 = *(Term*)curNode1->Pointer;
            Term subTerm2 = *(Term*)curNode2->Pointer;

            if(!termIsIdenticalToTerm(subTerm1, subTerm2)) return false;

            AdvanceListOfPointer(curNode2);
        }

        return true;
    }

}


void printTermWithNonterminalNames(Term term, FILE* stream, List nonterminalNames)
{

    if(term.Type == GTerminal)
    {
        fprintf(stream, "'%c'", term.Data.GTerminal);
    }
    else
    {
        assert(term.Data.GNonterminal.nonterminalType >= 0);
        assert((size_t)term.Data.GNonterminal.nonterminalType < nonterminalNames.Count);

        ListOfPointer* nameNode = (ListOfPointer*)listIndex(
            (size_t)term.Data.GNonterminal.nonterminalType, &nonterminalNames
        );
        fputs((const char*)nameNode->Pointer, stream);
    }

}
void printGroupWithNonterminalNames(Group group, FILE* stream, List nonterminalNames)
{

    if(group.Type == GTerm)
    {
        printTermWithNonterminalNames(group.Data.GTerm, stream, nonterminalNames);
    }
    else
    {
        if(group.Data.GDefinition.Count > 1)
            fputc('(', stream);

        iterateListOfPointerForward(group.Data.GDefinition, curNode)
        {
            Definition* definition = (Definition*)curNode->Pointer;
            printDefinitionWithNonterminalNames(*definition, stream, nonterminalNames);

            if(curNode->Header.Next != NULL)
            {
                fputs(" | ", stream);
            }
        }

        if(group.Data.GDefinition.Count > 1)
            fputc(')', stream);
    }

}
void printDefinitionWithNonterminalNames(Definition definition, FILE* stream, List nonterminalNames)
{

    if(definition.Groups.Count > 1 && (definition.lowerRepetition != 1 || definition.upperRepetition != 1))
        fputc('(', stream);

    iterateListOfPointerForward(definition.Groups, curNode)
    {
        Group* group = curNode->Pointer;
        printGroupWithNonterminalNames(*group, stream, nonterminalNames);

        if(curNode->Header.Next != NULL)
        {
            fputs(", ", stream);
        }
    }

    if(definition.Groups.Count > 1 && (definition.lowerRepetition != 1 || definition.upperRepetition != 1))
        fputc(')', stream);

    if(definition.lowerRepetition != 1 || definition.upperRepetition != 1)
        fprintf(stream, " * %zi-%zi", definition.lowerRepetition, definition.upperRepetition);

}
void printRuleWithNonterminalNames(Rule rule, FILE* stream, List nonterminalNames)
{

    iterateListOfPointerForward(rule.leftAlterations, curNode)
    {
        Definition* definition = (Definition*)curNode->Pointer;
        printDefinitionWithNonterminalNames(*definition, stream, nonterminalNames);

        if(curNode->Header.Next != NULL)
        {
            fputs(" | ", stream);
        }
    }

    fputs(" = ", stream);

    iterateListOfPointerForward(rule.rightAlterations, curNode)
    {
        Definition* definition = (Definition*)curNode->Pointer;
        printDefinitionWithNonterminalNames(*definition, stream, nonterminalNames);

        if(curNode->Header.Next != NULL)
        {
            fputs(" | ", stream);
        }
    }

    fputc(';', stream);

}
void printGrammarWithNonterminalNames(List grammar, FILE* stream, List nonterminalNames)
{

    iterateListOfPointerForward(grammar, curNode)
    {
        Rule* rule = (Rule*)curNode->Pointer;
        printRuleWithNonterminalNames(*rule, stream, nonterminalNames);
        fputc('\n', stream);
    }

}


const char* groupTypeToCString(GroupType type)
{

    switch(type)
    {
        case GTerm: return "Term";
        case GDefinition: return "Definition";
        default: return "Unknown";
    }

}
const char* termTypeToCString(TermType type)
{

    switch(type)
    {
        case GTerminal: return "Terminal";
        case GNonterminal: return "Nonterminal";
        default: return "Unknown";
    }

}


void finalizeTerm(Term* term)
{

    assert(term != NULL);

    if(term->Type == GNonterminal)
        finalizeListOfPointer(&term->Data.GNonterminal.Terms);

    *term = (struct Term){0};

}

void finalizeGroup(Group* group)
{

    assert(group != NULL);

    if(group->Type == GTerm) finalizeTerm(&group->Data.GTerm);
    else
    {
        iterateListOfPointerForward(group->Data.GDefinition, curNode)
        {
            finalizeDefinition((Definition*)curNode->Pointer);
        }

        finalizeListOfPointer(&group->Data.GDefinition);
    }

    *group = (struct Group){0};

}

void finalizeDefinition(Definition* def)
{

    assert(def != NULL);

    iterateListOfPointerForward(def->Groups, curNode)
    {
        Group* group = (Group*)curNode->Pointer;
        finalizeGroup(group);
        free(group);
    }

    finalizeListOfPointer(&def->Groups);

    *def = (struct Definition){0};

}

void finalizeRule(Rule* rule)
{

    assert(rule != NULL);

    iterateListOfPointerForward(rule->leftAlterations, curNode)
    {
        Definition* def = (Definition*)curNode->Pointer;
        finalizeDefinition(def);
        free(def);
    }
    iterateListOfPointerForward(rule->rightAlterations, curNode)
    {
        Definition* def = (Definition*)curNode->Pointer;
        finalizeDefinition(def);
        free(def);
    }

    finalizeListOfPointer(&rule->leftAlterations);
    finalizeListOfPointer(&rule->rightAlterations);

    *rule = (struct Rule){0};

}


void destroyDefinition(Definition* definition)
{

    finalizeDefinition(definition);
    free(definition);

}

void destroyGroup(Group* group)
{

    finalizeGroup(group);
    free(group);

}

void destroyRule(Rule* rule)
{

    finalizeRule(rule);
    free(rule);

}

