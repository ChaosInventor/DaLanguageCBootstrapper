#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "DaLanguageCBootstrap/String.h"
#include "DaLanguageCBootstrap/List.h"

//TODO: Entire implementation not optimal at the slightest

#define PUSH_SIZE_HEURISTIC 255

const String EMPTY_STRING = {0};

String initializeString(size_t size)
{

    assert(size > 0);

    String ret;

    StringNode* node = malloc(sizeof(StringNode));
    node->Header.Next = NULL;
    node->Header.Prev = NULL;
    assert(node != NULL);
    node->Buffer = malloc(sizeof(char) * size);
    assert(node->Buffer != NULL);
    node->BufferSize = size;
    node->UsedLower = 0;
    node->UsedUpper = 0;

    ret.Nodes.First = (ListNode*)node;
    ret.Nodes.Last = (ListNode*)node;
    ret.Nodes.Count = 1;
    ret.TotalCharacters = 0;

    return ret;

}
String initializeStringFromConst(const char* cString)
{

    assert(cString != NULL);

    String ret = {0};
    StringNode* node = malloc(sizeof(StringNode));
    assert(node != NULL);

    node->Header = (ListNode){0};
    node->Buffer = malloc(sizeof(char) * strlen(cString));
    assert(node->Buffer != NULL);
    node->BufferSize = strlen(cString);
    node->UsedLower = 0;
    node->UsedUpper = node->BufferSize;

    memcpy(node->Buffer, cString, node->BufferSize);

    ret.Nodes.First = &node->Header;
    ret.Nodes.Last = &node->Header;
    ret.Nodes.Count = 1;

    ret.TotalCharacters = node->BufferSize;

    return ret;

}
String initializeStringFromBuffer(char* buffer, size_t size)
{

    String ret = {0};

    if(buffer == NULL || size == 0)
    {
        return ret;
    }

    StringNode* node = malloc(sizeof(StringNode));
    assert(node != NULL);

    node->Header = (ListNode){0};
    node->Buffer = calloc(size, sizeof(char));
    assert(node->Buffer != NULL);
    node->BufferSize = size;
    node->UsedLower = 0;
    node->UsedUpper = size;

    for(size_t i = 0; i < size; ++i)
    {
        node->Buffer[i] = buffer[i];
    }

    ret.Nodes = (List){&node->Header, &node->Header, 1};
    ret.TotalCharacters = size;

    return ret;

}
String initializeStringFromCharacter(char c)
{
    return initializeStringFromBuffer(&c, 1);
}
String initializeStringFromPointer(void* p)
{
    //NOTE: Hacky, really hacky. Giant hack.
    return initializeStringFromBuffer((char*)&p, sizeof(void*));
}
String initializeEmptyString()
{
    return (struct String){0};
}
String cloneString(String from)
{

    String ret = {0};

    StringNode* node = malloc(sizeof(StringNode));
    assert(node != NULL);

    if(from.TotalCharacters == 0)
    {
        *node = (struct StringNode){0};
        ret.Nodes.First = &node->Header;
        ret.Nodes.Last = &node->Header;
        ret.Nodes.Count = 1;
        ret.TotalCharacters = 0;
        return ret;
    }
    else
    {
        node->Buffer = calloc(from.TotalCharacters, sizeof(char));
        assert(node->Buffer != NULL);
        node->BufferSize = from.TotalCharacters;
        node->Header.Next = NULL;
        node->Header.Prev = NULL;
        node->UsedLower = 0;
        node->UsedUpper = node->BufferSize;

        size_t i = 0;
        iterateStringNodesForward(from, curNode)
        {
            iterateStringNodeForward(curNode, curIndex, c)
            {
                node->Buffer[i] = *c;
                ++i;
            }
        }

        ret.Nodes.First = (ListNode*)node;
        ret.Nodes.Last = (ListNode*)node;
        ret.Nodes.Count = 1;

        ret.TotalCharacters = from.TotalCharacters;
    }

    return ret;

}

void stringNormalize(String* string)
{

    assert(string != NULL);

    StringNode* lastNode = NULL;
    size_t freeOnLast = 0;
    for(StringNode* curNode = (StringNode*)string->Nodes.First; curNode != NULL; curNode = (StringNode*)curNode->Header.Next)
    {

        if(curNode->UsedLower != 0 && curNode->UsedLower != curNode->UsedUpper)
        {
            if(freeOnLast > 0)
            {
                for(;lastNode->UsedUpper < lastNode->BufferSize && curNode->UsedLower < curNode->UsedUpper; ++lastNode->UsedUpper, ++curNode->UsedLower)
                {
                    lastNode->Buffer[lastNode->UsedUpper] = curNode->Buffer[curNode->UsedLower];
                }
                ++lastNode->UsedUpper;
            }
            size_t i, j;
            for(i = 0, j = curNode->UsedLower; i < curNode->BufferSize && j < curNode->UsedUpper; ++i, ++j)
            {
                curNode->Buffer[i] = curNode->Buffer[j];
            }

            curNode->UsedLower = 0;
            curNode->UsedUpper = i + 1;
        }

        lastNode = curNode;
        freeOnLast = (curNode->BufferSize - curNode->UsedUpper) + curNode->UsedLower;
    }

}

void pushCharToString(char push, String* string)
{

    assert(string != NULL);

    stringNormalize(string);

    for(StringNode* curNode = (StringNode*)string->Nodes.First; curNode != NULL; curNode = (StringNode*)curNode->Header.Next)
    {
        if(curNode->UsedUpper != curNode->BufferSize)
        {
            curNode->Buffer[curNode->UsedUpper++] = push;
            string->TotalCharacters++;
            return;
        }
    }

    StringNode* newNode = malloc(sizeof(StringNode));
    assert(newNode != NULL);
    newNode->Buffer = malloc(sizeof(char) * PUSH_SIZE_HEURISTIC);
    assert(newNode->Buffer != NULL);
    newNode->BufferSize = PUSH_SIZE_HEURISTIC;
    newNode->UsedLower = 0;
    newNode->UsedUpper = 1;
    newNode->Buffer[0] = push;

    string->TotalCharacters++;

    listPushBackNode(&newNode->Header, &string->Nodes);

}
void appendStringToString(String appendage, String* to)
{

    iterateStringNodesForward(appendage, curNode)
    {
        iterateStringNodeForward(curNode, i, c)
        {
            pushCharToString(*c, to);
        }
    }

}

bool stringIsEqualToString(String string1, String string2)
{

    if(string1.TotalCharacters != string2.TotalCharacters)
    {
        return false;
    }
    if(string1.TotalCharacters == 0 || string2.TotalCharacters == 0)
    {
        return string1.TotalCharacters == string2.TotalCharacters;
    }

    StringNode* curNode1 = (StringNode*)string1.Nodes.First;
    StringNode* curNode2 = (StringNode*)string2.Nodes.First;
    size_t i = curNode1->UsedLower, j = curNode2->UsedLower;
    while(curNode1 != NULL && curNode2 != NULL)
    {
        if(i >= curNode1->UsedUpper)
        {
            curNode1 = (StringNode*)curNode1->Header.Next;
            if(curNode1 == NULL)
            {
                //Reached the end, the strings are the same length, so j must also be on the last char.
                return true;
            }
            i = curNode1->UsedLower;
        }
        if(j >= curNode2->UsedUpper)
        {
            curNode2 = (StringNode*)curNode2->Header.Next;
            if(curNode2 == NULL)
            {
                //Reached the end, the strings are the same length, so j must also be on the last char.
                return true;
            }
            j = curNode2->UsedLower;
        }

        if(curNode1->Buffer[i] != curNode2->Buffer[j])
        {
            return false;
        }
        ++i;
        ++j;
    }

    return true;

}

StringNode* getFirstStringNode(String string)
{

    StringNode* first = (StringNode*)string.Nodes.First;
    while(first->UsedLower == first->UsedUpper)
    {
        if(first == (StringNode*)string.Nodes.Last)
        {
            break;
        }
        
        first = (StringNode*)first->Header.Next;

    }

    return first;
    
}
StringNode* getLastStringNode(String string)
{

    StringNode* last = (StringNode*)string.Nodes.Last;
    while(last->UsedLower == last->UsedUpper)
    {
        if(last == (StringNode*)string.Nodes.First)
        {
            break;
        }
        
        last = (StringNode*)last->Header.Prev;

    }

    return last;
    
}
size_t getFirstIndexStringNode(StringNode node)
{
    return node.UsedLower;
}
size_t getLastIndexStringNode(StringNode node)
{

    if(node.UsedUpper == 0)
    {
        return 0;
    }

    return node.UsedUpper - 1;

}

bool charIsAlphaNumeric(char c)
{

    return (c >= 0x30 && c <=0x39) || (c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a);

}

size_t writeString(String string, FILE* stream)
{

    size_t written = 0;

    iterateStringNodesForward(string, curNode)
    {
        size_t writtenNowTotal = 0;
        size_t charsInNode = curNode->UsedUpper - curNode->UsedLower;
        while(charsInNode != writtenNowTotal)
        {
            size_t writtenNow = fwrite(curNode->Buffer + curNode->UsedLower + writtenNowTotal, sizeof(char), charsInNode, stream);
            writtenNowTotal += writtenNow;
            charsInNode -= writtenNow;
            if(writtenNow == 0)
            {
                written += writtenNowTotal;
                return written;
            }
        }
        written += writtenNowTotal;
    }

    return written;

}
size_t readStringStart(String* string, FILE* stream)
{

    assert(string != NULL);

    stringNormalize(string);

    size_t red = 0;
    for(StringNode* curNode = (StringNode*)string->Nodes.First; curNode != NULL; curNode = (StringNode*)curNode->Header.Next)
    {
        while(curNode->UsedUpper != curNode->BufferSize)
        {
            size_t redNow = fread(curNode->Buffer + curNode->UsedLower, sizeof(char), (curNode->BufferSize - curNode->UsedUpper) + curNode->UsedLower, stream);

            if(redNow == 0)
            {
                string->TotalCharacters += red;
                return red;
            }

            red += redNow;

            curNode->UsedUpper += redNow;

        }
    }

    string->TotalCharacters += red;
    return red;

}

void emptyString(String* string)
{

    assert(string != NULL);

    for(StringNode* curNode = (StringNode*)string->Nodes.First; curNode != NULL; curNode = (StringNode*)curNode->Header.Next)
    {
        curNode->UsedLower = 0;
        curNode->UsedUpper = 0;
    }

    string->TotalCharacters = 0;

}

void finalizeString(String* string)
{

    assert(string != NULL);

    StringNode* lastNode = NULL;
    for(
            StringNode* curNode = (StringNode*)string->Nodes.First;
            curNode != NULL;
            curNode = (StringNode*)curNode->Header.Next
       )
    {

        free(curNode->Buffer);
        free(lastNode);

        lastNode = curNode;

    }

    free(lastNode);

    *string = (struct String){0};

}

