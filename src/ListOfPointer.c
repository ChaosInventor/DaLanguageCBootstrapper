#include "DaLanguageCBootstrap/ListOfPointer.h"
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

ListOfPointer* allocateListOfPointerNode()
{
    ListOfPointer* ret = malloc(sizeof(ListOfPointer));
    assert(ret != NULL);
    return ret;
}

ListOfPointer initializeListOfPointerNode(void* p)
{
    return (struct ListOfPointer){.Header = {0}, .Pointer = p};
}

ListOfPointer* createListOfPointerNode(void* p)
{
    ListOfPointer* ret = allocateListOfPointerNode();
    *ret = initializeListOfPointerNode(p);
    return ret;
}

List generateListOfPointer(void* pointer, ...)
{

    List ret = {0};

    va_list args;
    va_start(args, pointer);

    void* curPointer = pointer;
    while(curPointer != NULL)
    {
        appendPointerToListOfPointer(curPointer, &ret);
        curPointer = va_arg(args, void*);
    }

    return ret;

}

List copyListOfPointer(List list)
{

    List ret = {0};

    iterateListOfPointerForward(list, curNode)
    {
        ListOfPointer* new = createListOfPointerNode(curNode->Pointer);
        listPushFrontNode(&new->Header, &ret);
    }

    return ret;

}

void appendPointerToListOfPointer(void* p, List* list)
{
    listPushFrontNode(&createListOfPointerNode(p)->Header, list);
}

ListOfPointer* findFirstPointerInListOfPointer(void* pointer, List list)
{

    ListOfPointer* ret = NULL;

    iterateListOfPointerForward(list, curNode)
    {
        if(curNode->Pointer == pointer)
        {
            ret = curNode;
            break;
        }
    }

    return ret;

}
ListOfPointer* findLastPointerInListOfPointer(void* pointer, List list)
{

    ListOfPointer* ret = NULL;

    iterateListOfPointerBackward(list, curNode)
    {
        if(curNode->Pointer == pointer)
        {
            ret = curNode;
            break;
        }
    }

    return ret;

}

void finalizeListOfPointer(List* list)
{

    ListOfPointer* last = NULL;
    iterateListOfPointerForward(*list, curNode)
    {
        free(last);
        last = curNode;
    }
    free(last);

    *list = (struct List){0};

}

void destroyListOfPointer(List* list)
{
    finalizeListOfPointer(list);
    free(list);
}

