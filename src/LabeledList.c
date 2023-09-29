#include "DaLanguageCBootstrap/LabeledList.h"
#include <stdlib.h>
#include <assert.h>

LabeledListNode* allocateLabeledListNode()
{

    LabeledListNode* ret = malloc(sizeof(LabeledListNode));
    assert(ret != NULL);

    *ret = (struct LabeledListNode){0};

    return ret;

}

void finalizeLabeledList(List* list)
{

    assert(list != NULL);

    LabeledListNode* lastNode = NULL;
    iterateLabeledListForward(*list, curNode)
    {
        finalizeString(&curNode->Parameter);
        finalizeString(&curNode->Label);
        free(lastNode);
        lastNode = curNode;
    }
    free(lastNode);

    *list = (struct List){0};

}

