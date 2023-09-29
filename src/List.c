#include <assert.h>
#include <stdlib.h>
#include "DaLanguageCBootstrap/List.h"

List* allocateList()
{

    List* ret = malloc(sizeof(List));
    assert(ret != NULL);

    *ret = (struct List){0};

    return ret;

}

void listInsertNext(ListNode* node, ListNode* insert, List* list)
{

    assert(node != NULL);
    assert(insert != NULL);
    assert(list != NULL);

    node->Next = insert->Next;
    node->Prev = insert;
    insert->Next = node;

    if(node->Next != NULL)
    {
        node->Next->Prev = node;
    }
    else
    {
        list->Last = node;
        list->First = list->First == NULL? node : list->First;
    }

    list->Count++;

}
void listInsertPrev(ListNode* node, ListNode* insert, List* list)
{

    assert(node != NULL);
    assert(insert != NULL);
    assert(list != NULL);

    node->Next = insert;
    node->Prev = insert->Prev;
    insert->Prev = node;

    if(node->Prev != NULL)
    {
        node->Prev->Next = node;
    }
    else
    {
        list->First = node;
        list->Last = list->Last == NULL? node : list->Last;
    }

    list->Count++;

}
void listInsertAfterIndex(ListNode* node, size_t index, List* list)
{
    listInsertNext(node, listIndex(index, list), list);
}
void listInsertBeforeIndex(ListNode* node, size_t index, List* list)
{
    listInsertPrev(node, listIndex(index, list), list);
}
void listPushFrontNode(ListNode* node, List* list)
{

    assert(list != NULL);
    assert(node != NULL);

    if(list->Last == NULL || list->First == NULL)
    {
        list->First = node;
        list->Last = node;
        list->Count = 1;
        node->Next = NULL;
        node->Prev = NULL;
        return;
    }

    listInsertNext(node, list->Last, list);

}
void listPushBackNode(ListNode* node, List* list)
{
    assert(list != NULL);

    if(list->Last == NULL || list->First == NULL)
    {
        list->First = node;
        list->Last = node;
        list->Count = 1;
        node->Next = NULL;
        node->Prev = NULL;
        return;
    }

    listInsertPrev(node, list->First, list);
}

ListNode* listExtractNext(ListNode* node, List* list)
{

    assert(node != NULL);
    assert(list != NULL);

    ListNode* extracted = node->Next;

    if(extracted == NULL)
    {
        return NULL;
    }

    node->Next = extracted->Next;
    if(extracted->Next != NULL)
    {
        extracted->Next->Prev = node;
    }
    else
    {
        list->Last = node;
    }
    list->Count--;

    return extracted;

}
ListNode* listExtractPrev(ListNode* node, List* list)
{

    assert(node != NULL);
    assert(list != NULL);

    ListNode* extracted = node->Prev;

    if(extracted == NULL)
    {
        return NULL;
    }

    node->Prev = extracted->Prev;
    if(extracted->Prev != NULL)
    {
        extracted->Prev->Next = node;
    }
    else
    {
        list->First = node;
    }
    list->Count--;

    return extracted;

}
ListNode* listExtractAfterIndex(size_t index, List* list)
{
    return listExtractNext(listIndex(index, list), list);
}
ListNode* listExtractBeforeIndex(size_t index, List* list)
{
    return listExtractPrev(listIndex(index, list), list);
}
ListNode* listExtractNode(ListNode* node, List* list)
{
    
    assert(node != NULL);
    assert(list != NULL);

    if(node->Next != NULL)
    {
        return listExtractPrev(node->Next, list);
    }
    else if(node->Prev != NULL)
    {
        return listExtractNext(node->Prev, list);
    }
    else
    {
        list->First = NULL;
        list->Last = NULL;
        list->Count = 0;
        return node;
    }

}
ListNode* listPopFrontNode(List* list)
{

    assert(list != NULL);

    if(list->First == list->Last)
    {
        ListNode* toReturn = list->First;

        list->First = NULL;
        list->Last = NULL;
        list->Count = 0;

        return toReturn;
    }

    return listExtractNext(list->Last->Prev, list);

}
ListNode* listPopBackNode(List* list)
{

    assert(list != NULL);

    if(list->First == list->Last)
    {
        ListNode* toReturn = list->First;

        list->First = NULL;
        list->Last = NULL;
        list->Count = 0;

        return toReturn;
    }

    return listExtractPrev(list->First->Next, list);

}

void listLinkNodeAfterNode(ListNode* node1, ListNode* node2)
{

    assert(node1 != NULL);
    assert(node1->Prev == NULL);
    assert(node1->Next == NULL);
    assert(node2 != NULL);

    if(node2->Next != NULL) node2->Next->Prev = node1;
    node1->Next = node2->Next;
    node2->Next = node1;
    node1->Prev = node2;

}
void listLinkNodeBeforeNode(ListNode* node1, ListNode* node2)
{

    assert(node1 != NULL);
    assert(node1->Next == NULL);
    assert(node1->Prev == NULL);
    assert(node2 != NULL);

    if(node2->Prev != NULL) node2->Prev->Next = node1;
    node1->Prev = node2->Prev;
    node2->Prev = node1;
    node1->Next = node2;

}

void listMerge(List* from, List* to)
{

    assert(from != NULL);
    assert(to != NULL);

    //If to is empty, copy everything over
    if(to->Last == NULL)
    {
        to->First = from->First;
        to->Last = from->Last;
        to->Count = from->Count;
    }
    //If from is empty, do nothing
    else if(from->First != NULL)
    {
        to->Last->Next = from->First;
        from->First->Prev = to->Last;
        to->Count += from->Count;
    }

    *from = (struct List){0};

}

ListNode* listFirst(ListNode* node)
{

    assert(node != NULL);

    ListNode* First;
    for(First = node; First->Prev != NULL; First = First->Prev);

    return First;

}
ListNode* listLast(ListNode* node)
{

    assert(node != NULL);

    ListNode* Last;
    for(Last = node; Last->Next != NULL; Last = Last->Next);

    return Last;

}
ListNode* listIndex(size_t index, List* list)
{

    assert(index < list->Count);
    assert(list != NULL);

    ListNode* curNode = list->First;
    for(size_t i = 0; i <= index; ++i)
    {
        assert(curNode != NULL);
        curNode = curNode->Next;
    }

    return curNode;

}

List listInfer(ListNode* list)
{

    List infered;

    if(list == NULL)
    {
        infered.First = NULL;
        infered.Last = NULL;
        infered.Count = 0;

        return infered;
    }

    infered.Count = 0;
    ListNode* curNode = list;
    while (curNode != NULL)
    {
        curNode = curNode->Next;
        infered.Count++;
    }

    infered.Last = curNode;

    curNode = list->Prev;
    while(curNode != NULL)
    {
        curNode = curNode->Prev;
        infered.Count++;
    }

    infered.First = curNode;

    return infered;

}

bool listIsEmpty(List list)
{
    return list.First == NULL || list.Last == NULL || list.Count == 0;
}

void finalizeListNodeChainAfterNode(ListNode* node, List* list, void (*cleanUp)(ListNode*))
{

    assert(node != NULL);

    ListNode* last = NULL;
    ListNode* cur = node->Next;
    while(cur != NULL)
    {
        cleanUp(last);
        free(last);

        last = cur;
        cur = cur->Next;
        --list->Count;
    }

    cleanUp(last);
    free(last);

    node->Next = NULL;

    ++list->Count; //Since the first free is done on NULL
    list->Last = node;

}
void finalizeListNodeChainBeforeNode(ListNode* node, List* list, void (*cleanUp)(ListNode*))
{

    assert(node != NULL);

    ListNode* last = NULL;
    ListNode* cur = node->Prev;
    while(cur != NULL)
    {
        cleanUp(last);
        free(last);

        last = cur;
        cur = cur->Prev;
        --list->Count;
    }

    cleanUp(last);
    free(last);

    node->Prev = NULL;

    ++list->Count; //Since the first free is done on NULL
    list->First = node;

}

void destoryList(List* list, void (*cleanUp)(ListNode*))
{

    assert(list != NULL);

    ListNode* lastNode = NULL;
    ListNode* curNode = list->First;

    while(curNode != NULL)
    {
        cleanUp(lastNode);
        lastNode = curNode;
        curNode = curNode->Next;
    }

    *list = (List){0};

}

