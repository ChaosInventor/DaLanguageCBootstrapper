#ifndef DALCBOOT_LIST_OF_STRING
#define DALCBOOT_LIST_OF_STRING

#include "DaLanguageCBootstrap/String.h"
#include "DaLanguageCBootstrap/List.h"

#define iterateListOfStringNodeForward(curNode, startingValue)\
    iterateListNodeForwardType(curNode, startingValue, ListOfString, Header.Next)
#define iterateListOfStringNodeBackward(curNode, startingValue)\
    iterateListNodeBackwardType(curNode, startingValue, ListOfString, Header.Prev)

#define iterateListOfStringForward(listOfString, curNode)\
    iterateListOfStringNodeForward(curNode, (ListOfString*)(listOfString).First)
#define iterateListOfStringBackward(listOfString, curNode)\
    iterateListOfStringNodeBackward(curNode, (ListOfString*)(listOfString).Last)

#define NextListOfString(Node) (ListOfString*)(Node).Head.Next
#define PrevListOfString(Node) (ListOfString*)(Node).Head.Prev

#define AdvanceListOfString(Node) AdvanceListNodeType(Node, ListOfString*, Header.Next)
#define RetreatListOfString(Node) AdvanceListNodeType(Node, ListOfString*, Header.Prev)

typedef struct ListOfString
{

    ListNode Header;
    String Name;

} ListOfString;

List generateListOfString(size_t count, ...);

List copyListOfString(List list);

void appendStringToListOfString(String string, List* list);

bool listOfStringHasString(List list, String string);

ListOfString* findListOfStringWithName(List paramList, String name);

void finalizeListOfString(List* list);

void destroyListOfString(List* list);

#endif //DALCBOOT_LIST_OF_STRING
