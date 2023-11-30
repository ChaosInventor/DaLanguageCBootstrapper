#ifndef DALCBOOT_GRAMMAR
#define DALCBOOT_GRAMMAR

#include <stdio.h>
#include <stdbool.h>
#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/ListOfPointer.h"

typedef struct Rule
{

    List leftAlterations; //ListOfPointer of Definition
    List rightAlterations; //ListOfPointer of Definition

} Rule;

typedef struct Definition
{

    List Groups; //ListOfPointer of Group
    size_t lowerRepetition;
    size_t upperRepetition;

} Definition;

typedef enum TermType
{

    GTerminal,
    GNonterminal,

} TermType;
typedef struct Term
{

    TermType Type;
    union
    {
        char GTerminal;
        struct
        {
            int nonterminalType;
            List Terms; //ListOfPointer of Term
        } GNonterminal;
    } Data;

} Term;

typedef enum GroupType
{

    GTerm,
    GDefinition,

} GroupType;
typedef struct Group
{

    GroupType Type;
    union
    {
        Term GTerm;
        List GDefinition; //ListOfPointer of Definition*
    } Data;

} Group;

Rule* allocateRule();

Definition* allocateDefinition();

Group* allocateGroup();


Definition initializeDefinition();
Definition initializeDefinitionWithGroup(Group group);

Group initializeGroupFromTerm(Term term);

Term initializeTermFromTerminal(char terminal);
Term initializeTermFromNonterminalType(int type);


Definition* createDefinitionWithGroup(Group group);
Group* createGroupFromTerm(Term term);


void appendLeftAlternativeToRule(Definition left, Rule* rule);
void appendRightAlternativeToRule(Definition right, Rule* rule);

void appendGroupToDefinition(Group group, Definition* definition);


bool termIsEqualToTerm(Term t1, Term t2);
bool termIsIdenticalToTerm(Term t1, Term t2);


void printTerm(Term term, FILE* stream);


const char* groupTypeToCString(GroupType type);
const char* termTypeToCString(TermType type);


void finalizeTerm(Term* term);

void finalizeGroup(Group* group);

void finalizeDefinition(Definition* def);

void finalizeRule(Rule* rule);


void destroyDefinition(Definition* definition);

void destroyGroup(Group* group);

void destroyRule(Rule* rule);

#endif //DALCBOOT_GRAMMAR

