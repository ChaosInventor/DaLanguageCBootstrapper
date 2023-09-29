#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "DaLanguageCBootstrap/ListOfString.h"

List generateListOfString(size_t count, ...)
{

    List ret = {0};

    va_list args;
    va_start(args, count);

    for(size_t i = 0; i < count; ++i)
    {
        ListOfString* new = malloc(sizeof(ListOfString));
        assert(new != NULL);
        *new = (ListOfString){0};

        new->Name = initializeStringFromConst(va_arg(args, const char*));
        listPushFrontNode(&new->Header, &ret);
    }

    return ret;

}

List copyListOfString(List list)
{

    List ret = {0};

    iterateListForwardType(list, curNode, ListOfString, Header.Next)
    {
        ListOfString* newNode = malloc(sizeof(ListOfString));
        assert(newNode != NULL);
        *newNode = (struct ListOfString){0};

        newNode->Name = cloneString(curNode->Name);
        listPushFrontNode(&newNode->Header, &ret);
    }

    return ret;

}

void appendStringToListOfString(String string, List* list)
{

    ListOfString* node = malloc(sizeof(ListOfString));
    assert(node != NULL);

    *node = (struct ListOfString){0};

    node->Name = string;
    listPushFrontNode(&node->Header, list);

}

bool listOfStringHasString(List list, String string)
{

    return findListOfStringWithName(list, string) != NULL;

}

ListOfString* findListOfStringWithName(List paramList, String name)
{

    iterateListForwardType(paramList, param, ListOfString, Header.Next)
    {
        if(stringIsEqualToString(name, param->Name))
        {
            return param;
        }
    }

    return NULL;

}

void finalizeListOfString(List* list)
{

    assert(list != NULL);

    ListOfString* lastNode = NULL;
    iterateListOfStringForward(*list, curNode)
    {
        finalizeString(&curNode->Name); 
        free(lastNode);
        lastNode = curNode;
    }
    free(lastNode);

    *list = (struct List){0};

}

void destroyListOfString(List* list)
{

    finalizeListOfString(list);
    free(list);

}

