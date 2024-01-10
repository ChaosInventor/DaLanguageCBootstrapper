#ifndef DALCBOOT_DALANGUAGE_LIST_OF_POINTER
#define DALCBOOT_DALANGUAGE_LIST_OF_POINTER

#include "DaLanguageCBootstrap/ListOfPointer.h"
#include "DaLanguageCBootstrap/Instruction.h"
#include "DaLanguageCBootstrap/DaLanguageList.h"

DalInstruction(DaLanguageListOfPointerCreate);

DalInstruction(DaLanguageListOfPointerCopy);

DalInstruction(DaLanguageListOfPointerIsNull);

DalInstruction(DaLanguageListOfPointerNestedListInsert);
DalInstruction(DaLanguageListOfPointerNestedListExtract);

DalInstruction(DaLanguageListOfPointerDestroy);

#endif //DALCBOOT_DALANGUAGE_LIST_OF_POINTER
