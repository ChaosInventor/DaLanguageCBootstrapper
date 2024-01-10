#ifndef DALCBOOT_DALANGUAGE_LIST
#define DALCBOOT_DALANGUAGE_LIST

#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/Instruction.h"

typedef struct DaLanguageList
{

    List List;
    ListNode* curNode;

} DaLanguageList;

DalInstruction(DaLanguageListCreate);

DalInstruction(DaLanguageListNext);
DalInstruction(DaLanguageListPrev);
DalInstruction(DaLanguageListFirst);
DalInstruction(DaLanguageListLast);

DalInstruction(DaLanguageListSuffix);
DalInstruction(DaLanguageListPrefix);

DalInstruction(DaLanguageListUnsuffix);
DalInstruction(DaLanguageListUnprefix);
DalInstruction(DaLanguageListExtract);

DalInstruction(DaLanguageListDestroy);

#endif //DALCBOOT_DALANGUAGE_LIST
