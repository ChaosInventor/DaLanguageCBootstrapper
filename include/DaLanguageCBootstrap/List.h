#ifndef DALCBOOT_LIST
#define DALCBOOT_LIST

#include <stddef.h>
#include <stdbool.h>

#define iterateListNodeForwardType(curNode, startingValue, type, next)\
    for(type* curNode = startingValue; curNode != NULL; curNode = (type*)curNode->next)
#define iterateListNodeBackwardType(curNode, startingValue, type, prev)\
    for(type* curNode = startingValue; curNode != NULL; curNode = (type*)curNode->prev)

#define iterateListForwardType(list, curNode, type, next)\
    iterateListNodeForwardType(curNode, (type*)((list).First), type, next)
#define iterateListBackwardType(list, curNode, type, prev)\
    iterateListNodeBackwardType(curNode, (type*)((list).Last), type, prev)

#define AdvanceListNodeType(listNode, Type, Next) ((listNode) = (Type)((listNode)->Next))
#define RetreatListNodeType(listNode, Type, Prev) ((listNode) = (Type)((listNode)->Prev))

typedef struct ListNode
{
    struct ListNode* Next;
    struct ListNode* Prev;
} ListNode;

typedef struct List
{
    ListNode* First;
    ListNode* Last;
    size_t Count;
} List;

List* allocateList(void);

void listInsertNext(ListNode* node, ListNode* insert, List* list);
void listInsertPrev(ListNode* node, ListNode* insert, List* list);
void listInsertAfterIndex(ListNode* node, size_t index, List* list);
void listInsertBeforeIndex(ListNode* node, size_t index, List* list);
void listPushFrontNode(ListNode* node, List* list);
void listPushBackNode(ListNode* node, List* list);

ListNode* listExtractNext(ListNode* node, List* list);
ListNode* listExtractPrev(ListNode* node, List* list);
ListNode* listExtractAfterIndex(size_t index, List* list);
ListNode* listExtractBeforeIndex(size_t index, List* list);
ListNode* listExtractNode(ListNode* node, List* list);
ListNode* listPopFrontNode(List* list);
ListNode* listPopBackNode(List* list);

void listLinkNodeAfterNode(ListNode* node1, ListNode* node2);
void listLinkNodeBeforeNode(ListNode* node1, ListNode* node2);

void listMerge(List* from, List* to);

ListNode* listFirst(ListNode* node);
ListNode* listLast(ListNode* node);
ListNode* listIndex(size_t index, List* list);

List listInfer(ListNode* list);

bool listIsEmpty(List list);

void finalizeListNodeChainAfterNode(ListNode* node, List* list, void (*cleanUp)(ListNode*));
void finalizeListNodeChainBeforeNode(ListNode* node, List* list, void (*cleanUp)(ListNode*));

void destoryList(List* list, void (*cleanUp)(ListNode*));

#endif //DALCBOOT_LIST

