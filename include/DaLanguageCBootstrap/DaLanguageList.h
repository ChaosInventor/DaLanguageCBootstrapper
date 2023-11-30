#ifndef DALANGUAGE_LIST
#define DALANGUAGE_LIST

#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/Instruction.h"

typedef struct DaLanguageList
{

    List list;
    ListNode* curNode;

} DaLanguageList;

DalInstruction(DaLanguageListNext);
DalInstruction(DaLanguageListPrev);
DalInstruction(DaLanguageListFirst);
DalInstruction(DaLanguageListLast);

#endif //DALANGUAGE_LIST
