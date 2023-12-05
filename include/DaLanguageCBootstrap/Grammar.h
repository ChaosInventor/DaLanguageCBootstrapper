#ifndef DALCBOOT_GRAMMAR
#define DALCBOOT_GRAMMAR

#include <stdio.h>
#include <stdbool.h>
#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/ListOfPointer.h"

#define ruleEnd NULL

#define grammar(...)\
    generateListOfPointer(__VA_ARGS__)

#define rule(leftAlterations, rightAlterations)\
    createRuleFromLeftAlterationsAndRightAlterations(\
            generateListOfPointer leftAlterations,\
            generateListOfPointer rightAlterations\
        )

#define ruleDefinition(groups, lower, upper)\
    createDefinitionWithGroupsAndRepetition(generateListOfPointer groups, lower, upper)
#define ruleDefinitionOnce(...)\
    ruleDefinition((__VA_ARGS__), 1, 1)

#define ruleGroup(...)\
    createGroupFromDefinitions(generateListOfPointer(__VA_ARGS__))

#define ruleNonterminal(type)\
    createGroupFromTerm(initializeTermFromNonterminalType(type))
#define ruleOnlynonterminal(type)\
    createDefinitionWithGroup(initializeGroupFromTerm(initializeTermFromNonterminalType(type)))

#define ruleNonterminalByName(names, name)\
    ruleNonterminal(memoizeNonterminalTypeInListOfNamesWithName(names, name))
#define ruleOnlyNonterminalByName(names, name)\
    ruleOnlynonterminal(memoizeNonterminalTypeInListOfNamesWithName(names, name))

#define ruleTerminal(terminal)\
    createGroupFromTerm(initializeTermFromTerminal(terminal))
#define ruleOnlyTerminal(terminal)\
    createDefinitionWithGroup(initializeGroupFromTerm(initializeTermFromTerminal(terminal)))

#define ruleOptional(...)\
    ruleGroup(ruleDefinition((__VA_ARGS__), 0, 1), ruleEnd)
#define ruleAny(...)\
    ruleGroup(ruleDefinition((__VA_ARGS__), 0, 0), ruleEnd)

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


Rule initializeRuleFromLeftAlterationsAndRightAlterations(List left, List right);

Definition initializeDefinition();
Definition initializeDefinitionWithGroupsAndRepetition(
    List groups, size_t lower, size_t upper
);
Definition initializeDefinitionWithGroup(Group group);

Group initializeGroupFromTerm(Term term);
Group initializeGroupFromDefinitions(List definitons);

Term initializeTermFromTerminal(char terminal);
Term initializeTermFromNonterminalType(int type);


Rule* createRuleFromLeftAlterationsAndRightAlterations(List left, List right);

Definition* createDefinitionWithGroup(Group group);
Definition* createDefinitionWithGroupsAndRepetition(
    List groups, size_t lower, size_t upper
);

Group* createGroupFromTerm(Term term);
Group* createGroupFromDefinitions(List definitions);


void appendLeftAlternativeToRule(Definition left, Rule* rule);
void appendRightAlternativeToRule(Definition right, Rule* rule);

void appendGroupToDefinition(Group group, Definition* definition);


int memoizeNonterminalTypeInListOfNamesWithName(List* names, const char* name);


bool termIsEqualToTerm(Term t1, Term t2);
bool termIsIdenticalToTerm(Term t1, Term t2);


void printTermWithNonterminalNames(Term term, FILE* stream, List nonterminalNames);
void printGroupWithNonterminalNames(Group group, FILE* stream, List nonterminalNames);
void printDefinitionWithNonterminalNames(Definition definition, FILE* stream, List nonterminalNames);
void printRuleWithNonterminalNames(Rule rule, FILE* stream, List nonterminalNames);
void printGrammarWithNonterminalNames(List grammar, FILE* stream, List nonterminalNames);


const char* groupTypeToCString(GroupType type);
const char* termTypeToCString(TermType type);


void finalizeTerm(Term* term);

void finalizeGroup(Group* group);

void finalizeDefinition(Definition* def);

void finalizeRule(Rule* rule);

void finalizeGrammar(List* rules);


void destroyDefinition(Definition* definition);

void destroyGroup(Group* group);

void destroyRule(Rule* rule);

#endif //DALCBOOT_GRAMMAR
