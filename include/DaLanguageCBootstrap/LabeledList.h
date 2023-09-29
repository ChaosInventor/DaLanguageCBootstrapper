#ifndef DALCBOOT_LABELED_LIST
#define DALCBOOT_LABELED_LIST

#define iterateLabeledListForward(list, curNode)\
    iterateListForwardType(list, curNode, LabeledListNode, Header.Next)
#define iterateLabeledListBackward(list, curNode)\
    iterateListBackwardType(list, curNode, LabeledListNode, Header.Next)

#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/String.h"

typedef struct LabeledListNode
{

    ListNode Header;
    String Parameter;
    String Label;

} LabeledListNode;

LabeledListNode* allocateLabeledListNode(void);

void finalizeLabeledList(List* list);

#endif //DALCBOOT_LABELED_LIST

