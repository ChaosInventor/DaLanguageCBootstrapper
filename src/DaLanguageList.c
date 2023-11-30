#include "DaLanguageCBootstrap/DaLanguageList.h"

DalInstruction(DaLanguageListNext)
{

    DalValue(dList, DaLanguageList);
    DalJump(more);
    DalJump(end);

    if(dList->curNode == NULL) return end;
    else
    {
        AdvanceListNodeType(dList->curNode, ListNode*, Next);

        if(dList->curNode == NULL) return end;
        else return more;
    }

}
DalInstruction(DaLanguageListPrev)
{

    DalValue(dList, DaLanguageList);
    DalJump(more);
    DalJump(end);

    if(dList->curNode == NULL) return end;
    else
    {
        RetreatListNodeType(dList->curNode, ListNode*, Next);

        if(dList->curNode == NULL) return end;
        else return more;
    }

}
DalInstruction(DaLanguageListFirst)
{

    DalValue(dList, DaLanguageList);
    DalJump(notNull);
    DalJump(null);

    dList->curNode = dList->list.First;

    if(dList->curNode == NULL) return null;
    else return notNull;

}
DalInstruction(DaLanguageListLast)
{

    DalValue(dList, DaLanguageList);
    DalJump(notNull);
    DalJump(null);

    dList->curNode = dList->list.Last;

    if(dList->curNode == NULL) return null;
    else return notNull;

}

