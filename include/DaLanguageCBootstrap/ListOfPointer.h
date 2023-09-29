#ifndef DALCBOOT_LIST_OF_POINTER
#define DALCBOOT_LIST_OF_POINTER

#include "DaLanguageCBootstrap/List.h"

#define iterateListOfPointerNodeForward(curNode, startingValue)\
    iterateListNodeForwardType(curNode, startingValue, ListOfPointer, Header.Next)
#define iterateListOfPointerNodeBackward(curNode, startingValue)\
    iterateListNodeBackwardType(curNode, startingValue, ListOfPointer, Header.Prev)

#define iterateListOfPointerForward(listOfPointer, curNode)\
    iterateListOfPointerNodeForward(curNode, (ListOfPointer*)(listOfPointer).First)
#define iterateListOfPointerBackward(listOfPointer, curNode)\
    iterateListOfPointerNodeBackward(curNode, (ListOfPointer*)(listOfPointer).Last)

#define NextListOfPointer(Node) ((ListOfPointer*)(Node).Header.Next)
#define PrevListOfPointer(Node) ((ListOfPointer*)(Node).Header.Prev)

#define FirstListOfPointer(list) ((ListOfPointer*)(list).First)
#define LastListOfPointer(list) ((ListOfPointer*)(list).Last)

#define AdvanceListOfPointer(Node) AdvanceListNodeType(Node, ListOfPointer*, Header.Next)
#define RetreatListOfPointer(Node) AdvanceListNodeType(Node, ListOfPointer*, Header.Prev)

#define FirstPointerListOfPointerType(list, type) ((type*)FirstListOfPointer(list)->Pointer)
#define LastPointerListOfPointerType(list, type) ((type*)LastListOfPointer(list)->Pointer)

typedef struct ListOfPointer
{

    ListNode Header;
    void* Pointer;

} ListOfPointer;

ListOfPointer* allocateListOfPointerNode(void);

ListOfPointer initializeListOfPointerNode(void* p);

ListOfPointer* createListOfPointerNode(void* p);

List copyListOfPointer(List list);

void appendPointerToListOfPointer(void* p, List* list);

ListOfPointer* findFirstPointerInListOfPointer(void* pointer, List list);
ListOfPointer* findLastPointerInListOfPointer(void* pointer, List list);

void finalizeListOfPointer(List* list);

void destroyListOfPointer(List* list);

#endif //DALCBOOT_LIST_OF_POINTER

