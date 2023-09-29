#ifndef DALCBOOT_STRING
#define DALCBOOT_STRING

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "DaLanguageCBootstrap/List.h"

#define stringNodeIsTerminal(node) node == NULL
#define advanceStringNode(node) node = (StringNode*)node->Header.Next
#define retreatStringNode(node) node = (StringNode*)node->Header.Prev

#define iterateStringNodesForward(string, curNode)\
    iterateListForwardType((string).Nodes, curNode, StringNode, Header.Next)
#define iterateStringNodesBackward(string, curNode)\
    iterateListBackwardType((string).Nodes, curNode, StringNode, Header.Next)

#define iterateStringNodeForward(node, curIndex, curChar)\
        char* curChar;\
        curChar = curNode->Buffer;\
        for(size_t curIndex = curNode->UsedLower; curIndex < curNode->UsedUpper; ++curIndex, ++curChar)
#define iterateStringNodeBackward(node, curIndex, curChar)\
        char* curChar;\
        curChar = node->Buffer + node->UsedUpper - 1;\
        for(size_t curIndex = node->UsedUpper; curIndex-- > node->UsedLower; --curChar)

#define ConstString(name, value)\
    static char name##Value[] = value;\
    static StringNode name##Node = {{0}, (char*)name##Value, sizeof(name##Value) - 1, 0, sizeof(name##Value) - 1};\
    static String name = {{&name##Node.Header, &name##Node.Header, 1}, sizeof(name##Value) - 1};

typedef struct StringNode
{

    ListNode Header;
    char* Buffer;
    size_t BufferSize;
    size_t UsedLower;
    size_t UsedUpper;

} StringNode;

typedef struct String
{

    List Nodes;
    size_t TotalCharacters;

} String;

extern const String EMPTY_STRING;

StringNode initializeStringNode(size_t size);
String initializeString(size_t size);
String initializeStringFromConst(const char* cString);
String initializeStringFromBuffer(char* buffer, size_t size);
String initializeStringFromCharacter(char c);
String initializeStringFromPointer(void* p);
String initializeEmptyString(void);
String cloneString(String from);

void addStringAfter(String toAdd, size_t index, String* string);
void addStringBefore(String toAdd, size_t index, String* string);
void pushCharToString(char push, String* string);
void appendStringToString(String appendage, String* to);

String* extractString(String* string, size_t lowerIndex, size_t upperIndex);
void removeString(String* string, size_t lowerIndex, size_t upperIndex);
char popString(String* string);
String popStringMultiple(String* string, size_t amount);

char getStringIndexChar(String string, size_t index);
StringNode* getStringIndexNode(String string, size_t index);
char getStringNodeIndex(StringNode node, size_t index);

StringNode* getFirstStringNode(String string);
StringNode* getLastStringNode(String string);
size_t getFirstIndexStringNode(StringNode node);
size_t getLastIndexStringNode(StringNode node);

size_t lastIndexString(String string);
size_t findSubstringFoward(String haystack, String needle, size_t startIndex);
size_t findSubstringBackward(String haystack, String needle, size_t startIndex);
size_t findCharactersFoward(String string, String characters, size_t startIndex);
size_t findCharactersBackward(String string, String characters, size_t startIndex);
size_t findFreeSpaceInStringNode(StringNode);
size_t findFullSpaceInStringNode(StringNode);

bool stringIsEqualToString(String string1, String string2);
bool stringIsEmpty(String string);

size_t writeString(String string, FILE* stream);
size_t readStringStart(String* string, FILE* stream);
size_t readStringEnd(String* string, FILE* stream);

bool charIsAlpha(char);
bool charIsNumeric(char);
bool charIsAlphaNumeric(char);
bool charIsPrintable(char);

void emptyString(String* string);
void finalizeString(String* string);

#endif //DALCBOOT_STRING

