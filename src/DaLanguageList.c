#include "DaLanguageCBootstrap/DaLanguageList.h"
#include <stdlib.h>

DalInstruction(DaLanguageListCreate)
{

    DalValue(pointer, DaLanguageList*);
    DalJump(next);

    *pointer = malloc(sizeof(DaLanguageList));
    assert(*pointer != NULL);

    **pointer = (struct DaLanguageList){0};

    return next;

}

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
        RetreatListNodeType(dList->curNode, ListNode*, Prev);

        if(dList->curNode == NULL) return end;
        else return more;
    }

}
DalInstruction(DaLanguageListFirst)
{

    DalValue(dList, DaLanguageList);
    DalJump(notNull);
    DalJump(null);

    dList->curNode = dList->List.First;

    if(dList->curNode == NULL) return null;
    else return notNull;

}
DalInstruction(DaLanguageListLast)
{

    DalValue(dList, DaLanguageList);
    DalJump(notNull);
    DalJump(null);

    dList->curNode = dList->List.Last;

    if(dList->curNode == NULL) return null;
    else return notNull;

}

DalInstruction(DaLanguageListSuffix)
{

    DalValue(dList, DaLanguageList);
    DalValue(node, ListNode*);
    DalJump(notNull);
    DalJump(null);

    if(dList->curNode == NULL) dList->curNode = dList->List.Last;
    if(dList->curNode == NULL)
    {
        listPushFrontNode(*node, &dList->List);
        dList->curNode = *node;
        return null;
    }

    listInsertNext(*node, dList->curNode, &dList->List);

    return notNull;

}
DalInstruction(DaLanguageListPrefix)
{

    DalValue(dList, DaLanguageList);
    DalValue(node, ListNode*);
    DalJump(notNull);
    DalJump(null);

    if(dList->curNode == NULL) dList->curNode = dList->List.First;
    if(dList->curNode == NULL)
    {
        listPushBackNode(*node, &dList->List);
        dList->curNode = *node;
        return null;
    }

    listInsertPrev(*node, dList->curNode, &dList->List);

    return notNull;

}

DalInstruction(DaLanguageListUnsuffix)
{

    DalValue(dList, DaLanguageList);
    DalValue(node, ListNode*);
    DalJump(notNull);
    DalJump(null);

    if(dList->curNode == NULL)
    {
        *node = NULL;
        return null;
    }
    if(dList->curNode->Next == NULL)
    {
        *node = listPopFrontNode(&dList->List);
        dList->curNode = dList->curNode->Prev;
        return notNull;
    }

    *node = listExtractNext(dList->curNode, &dList->List);

    return notNull;

}
DalInstruction(DaLanguageListUnprefix)
{

    DalValue(dList, DaLanguageList);
    DalValue(node, ListNode*);
    DalJump(notNull);
    DalJump(null);

    if(dList->curNode == NULL)
    {
        *node = NULL;
        return null;
    }
    if(dList->curNode->Prev == NULL)
    {
        *node = listPopBackNode(&dList->List);
        dList->curNode = dList->curNode->Next;
        return notNull;
    }

    *node = listExtractPrev(dList->curNode, &dList->List);

    return notNull;

}
DalInstruction(DaLanguageListExtract)
{

    DalValue(dList, DaLanguageList);
    DalValue(node, ListNode*);
    DalJump(notNull);
    DalJump(null);

    if(dList->curNode == NULL)
    {
        *node = NULL;
        return null;
    }

    *node = listExtractNode(dList->curNode, &dList->List);
    dList->curNode = NULL;

    return notNull;

}

static void nodeCleanUp(ListNode* p) { free(p); return; }
DalInstruction(DaLanguageListDestroy)
{

    DalValue(pointer, DaLanguageList*);
    DalJump(next);

    destoryList(&(*pointer)->List, nodeCleanUp);
    free(*pointer);

    *pointer = NULL;

    return next;

}
