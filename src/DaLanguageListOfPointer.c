#include "DaLanguageCBootstrap/DaLanguageListOfPointer.h"
#include <stdlib.h>
#include <assert.h>

DalInstruction(DaLanguageListOfPointerCreate)
{

    DalValue(node, ListOfPointer*);
    DalJump(next);

    *node = malloc(sizeof(ListOfPointer));
    assert(*node != NULL);

    **node = (struct ListOfPointer){0};

    return next;

}

DalInstruction(DaLanguageListOfPointerCopy)
{

    DalValue(from, DaLanguageList);
    DalValue(to, DaLanguageList);
    DalJump(next);

    ListOfPointer* fromNode = (ListOfPointer*)from->curNode;
    ListOfPointer* toNode = (ListOfPointer*)to->curNode;

    toNode->Pointer = fromNode->Pointer;

    return next;

}

DalInstruction(DaLanguageListOfPointerIsNull)
{

    DalValue(dList, DaLanguageList);
    DalJump(null);
    DalJump(notNull);

    ListOfPointer* node = (ListOfPointer*)dList->curNode;

    if(node->Pointer == NULL) return null;
    else return notNull;

}

DalInstruction(DaLanguageListOfPointerNestedListInsert)
{

    DalValue(nested, DaLanguageList);
    DalValue(dList, DaLanguageList);
    DalJump(next);

    if(nested->curNode == NULL) return next;

    ListOfPointer* pointerNode = (ListOfPointer*)nested->curNode;
    DaLanguageList* nestedList = (DaLanguageList*)pointerNode->Pointer;

    *nestedList = *dList;

    return next;

}
DalInstruction(DaLanguageListOfPointerNestedListExtract)
{

    DalValue(nested, DaLanguageList);
    DalValue(dList, DaLanguageList);
    DalJump(next);

    if(nested->curNode == NULL) return next;

    ListOfPointer* pointerNode = (ListOfPointer*)nested->curNode;
    DaLanguageList* nestedList = (DaLanguageList*)pointerNode->Pointer;

    *dList = *nestedList;

    return next;

}

DalInstruction(DaLanguageListOfPointerDestroy)
{

    DalValue(node, ListOfPointer*);
    DalJump(next);

    free(*node);
    *node = NULL;

    return next;

}
