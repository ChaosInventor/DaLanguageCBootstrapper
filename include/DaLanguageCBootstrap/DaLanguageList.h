#ifndef DALCBOOT_DALANGUAGE_LIST
#define DALCBOOT_DALANGUAGE_LIST

#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/Instruction.h"

typedef struct DaLanguageList
{

    List List;
    ListNode* curNode;

} DaLanguageList;

DalInstruction(DaLanguageListNext);
DalInstruction(DaLanguageListPrev);
DalInstruction(DaLanguageListFirst);
DalInstruction(DaLanguageListLast);

#endif //DALCBOOT_DALANGUAGE_LIST
