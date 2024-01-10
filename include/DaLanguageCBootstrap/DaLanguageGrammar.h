#ifndef DALCBOOT_DALANGUAGE_GRAMMAR
#define DALCBOOT_DALANGUAGE_GRAMMAR

#include "DaLanguageCBootstrap/Instruction.h"
#include "DaLanguageCBootstrap/DaLanguageList.h"
#include "DaLanguageCBootstrap/Grammar.h"

DalInstruction(DaLanguageGrammarRuleLeft);
DalInstruction(DaLanguageGrammarRuleRight);

DalInstruction(DaLanguageGrammarDefinitionGroups);

DalInstruction(DaLanguageGrammarGroupType);
DalInstruction(DaLanguageGrammarGroupDefinitions);

DalInstruction(DaLanguageGrammarTermType);

DalInstruction(DaLanguageGrammarPrintTerm);

#endif //DALCBOOT_DALANGUAGE_GRAMMAR
