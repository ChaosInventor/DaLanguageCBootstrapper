#include <assert.h>
#include <stdlib.h>
#include "DaLanguageCBootstrap/PT.h"

PTNode* allocatePTNode(void)
{

    PTNode* ret = malloc(sizeof(PTNode));
    assert(ret != NULL);

    *ret = (struct PTNode){0};

    return ret;

}


PT initializePTWithRoot()
{

    PT ret = {0};

    PTNode* root = allocatePTNode();

    root->Type = PRoot;
    root->Depth = 0;

    ret.Tree.Root = (TreeNode*)root;
    ret.Tree.Count = 1;
    ret.Height = 0;
    ret.maxDegree = 0;

    return ret;

}

void initializePTNodeFromConstAt(PTNode* ret, const char* data, PTType type, PT* pt, PTNode* root)
{

    assert(pt != NULL);
    assert(root != NULL);
    assert(root != NULL);

    *ret = (struct PTNode){0};;

    ret->Type = type;
    ret->Data = initializeStringFromConst(data);

    pushPTNodeToFront(ret, root, pt);

}
void initializePTNameNodeAt(PTNode* ret, String data, PT* pt, PTNode* root)
{

    assert(pt != NULL);
    assert(root != NULL);
    assert(!stringIsEqualToString(data, EMPTY_STRING));

    *ret = (struct PTNode){0};;

    ret->Type = PName;
    ret->Data = cloneString(data);

    pushPTNodeToFront(ret, root, pt);

}
void initializePTLabelNodeAt(PTNode* ret, String data, PT* pt, PTNode* root)
{

    assert(pt != NULL);
    assert(root != NULL);
    assert(!stringIsEqualToString(data, EMPTY_STRING));

    *ret = (struct PTNode){0};;

    ret->Type = PLabel;
    ret->Data = cloneString(data);

    pushPTNodeToFront(ret, root, pt);

}
void initializePTNodeFromListOfStringAt(PTNode* ret, List list, PTType type, PT* pt, PTNode* root)
{

    assert(type == PUnlabeledJumpList || type == PUnlabeledValueList);
    assert(pt != NULL);
    assert(root != NULL);

    *ret = (struct PTNode){0};;

    ret->Type = type;
    ret->Data = initializeEmptyString();

    pushPTNodeToFront(ret, root, pt);

    PTNode* node = allocatePTNode();
    if(type == PUnlabeledValueList)
        initializePTNodeFromConstAt(node, "(", PValueListStart, pt, ret);
    else
        initializePTNodeFromConstAt(node, "[", PJumpListStart, pt, ret);

    iterateListOfStringForward(list, curNode)
    {
        node = allocatePTNode();
        initializePTNameNodeAt(node, curNode->Name, pt, ret);

        node = allocatePTNode();
        initializePTNodeFromConstAt(node, ",", PSeparator, pt, ret);
    }

    node = allocatePTNode();
    if(type == PUnlabeledValueList)
        initializePTNodeFromConstAt(node, ")", PValueListEnd, pt, ret);
    else
        initializePTNodeFromConstAt(node, "]", PJumpListEnd, pt, ret);

}
void initializePTNodeFromLabeledListAt(PTNode* ret, List list, PTType type, PT* pt, PTNode* root)
{

    assert(type == PLabeledJumpList || type == PLabeledValueList);
    assert(pt != NULL);
    assert(root != NULL);

    *ret = (struct PTNode){0};;

    ret->Type = type;
    ret->Data = initializeEmptyString();

    pushPTNodeToFront(ret, root, pt);

    PTNode* node = allocatePTNode();
    if(type == PLabeledValueList)
        initializePTNodeFromConstAt(node, "(", PValueListStart, pt, ret);
    else
        initializePTNodeFromConstAt(node, "[", PJumpListStart, pt, ret);

    iterateLabeledListForward(list, curNode)
    {
        PTNode* parent = ret;
        node = allocatePTNode();
        if(!stringIsEqualToString(curNode->Label, EMPTY_STRING))
        {
            initializePTLabelNodeAt(node, curNode->Label, pt, ret);
            parent = node;
            node = allocatePTNode();
        }

        initializePTNameNodeAt(node, curNode->Parameter, pt, parent);

        node = allocatePTNode();
        initializePTNodeFromConstAt(node, ",", PSeparator, pt, ret);
    }

    node = allocatePTNode();
    if(type == PLabeledValueList)
        initializePTNodeFromConstAt(node, ")", PValueListEnd, pt, ret);
    else
        initializePTNodeFromConstAt(node, "]", PJumpListEnd, pt, ret);

}

List initializeListOfStringFromPTNode(PTNode* list)
{

    List ret = {0};

    if(list == NULL) return ret;

    String name = initializeEmptyString();
    iteratePTNodeChildrenForward(*list, curNode)
    {
        //Join together names that are separated by ignorables.
        if(curNode->Type == PName)
        {
            appendStringToString(curNode->Data, &name);
        }
        else if(curNode->Type == PSeparator)
        {
            //Note: append does not make a copy of the string, it uses the exact
            //same one.
            appendStringToListOfString(name, &ret);
            name = initializeEmptyString();
        }
    }

    //If there isn't a trailing "," on the last name
    if(!stringIsEqualToString(name, EMPTY_STRING))
    {
        appendStringToListOfString(name, &ret);
    }

    return ret;

}
List initializeLabeledListFromPTNode(PTNode* list)
{

    List ret = {0};

    if(list == NULL) return ret;

    LabeledListNode* lastNode = allocateLabeledListNode();

    String accumulator = initializeEmptyString();
    iteratePTNodeChildrenForward(*list, curNode)
    {
        if(curNode->Type == PName)
        {
            appendStringToString(curNode->Data, &accumulator);
        }
        else if(curNode->Type == PLabel)
        {
            appendStringToString(curNode->Data, &accumulator);
            lastNode->Label = accumulator;

            accumulator = initializeEmptyString();
            appendStringToString(PTNodeLastChild(*curNode)->Data, &accumulator);
        }
        else if(curNode->Type == PSeparator)
        {
            lastNode->Parameter = accumulator;

            accumulator = initializeEmptyString();
            listPushFrontNode(&lastNode->Header, &ret);
            lastNode = allocateLabeledListNode();
        }
    }

    if(!stringIsEqualToString(accumulator, EMPTY_STRING))
    {
        lastNode->Parameter = accumulator;
        listPushFrontNode(&lastNode->Header, &ret);
    }
    //Empty list in parser tree
    else if(ret.Count == 0)
    {
        free(lastNode);
    }

    return ret;

}


PTNode* createPTNodeType(PTType type, PT* pt, PTNode* root)
{

    assert(pt != NULL);
    assert(root != NULL);

    PTNode* ret = allocatePTNode();

    ret->Type = type;

    pushPTNodeToFront(ret, root, pt);

    return ret;

}


bool ptNodeIsExecutable(PTNode node)
{

    PTNode* inspect = &node;

    if(inspect->Type == PLabel)
    {
        PTNodeDescendEnd(inspect);
    }

    return inspect->Type == PSpec;

}
bool ptNodeHasChildOfType(PTNode node, PTType type)
{

    iteratePTNodeChildrenForward(node, curNode)
    {
        if(curNode->Type == type)
        {
            return true;
        }
    }

    return false;

}


void pushPTNodeToFront(PTNode* node, PTNode* root, PT* pt)
{

    assert(node != NULL);
    assert(root != NULL);
    assert(pt != NULL);

    node->Depth = root->Depth + 1;

    pushTreeNodeToFrontInNodeTree(&node->Header, &root->Header, &pt->Tree);

    if(node->Depth > pt->Height)
    {
        pt->Height = node->Depth;
    }
    if(root->Header.Children.Count > pt->maxDegree)
    {
        pt->maxDegree = root->Header.Children.Count;
    }

}
void pushPTNodeToBack(PTNode* node, PTNode* root, PT* pt)
{

    assert(node != NULL);
    assert(root != NULL);
    assert(pt != NULL);

    node->Depth = root->Depth + 1;

    pushTreeNodeToBackInNodeTree(&node->Header, &root->Header, &pt->Tree);

    if(node->Depth > pt->Height)
    {
        pt->Height = node->Depth;
    }
    if(root->Header.Children.Count > pt->maxDegree)
    {
        pt->maxDegree = root->Header.Children.Count;
    }

}


PTNode* findNextNodeInDef(PTNode node)
{

    PTNode* curNode = &node;
    while(PTNodeParent(*curNode)->Type != PDefBody)
    {
        PTNodeAscend(curNode);
    }

    do
    {
        PTNodeAdvance(curNode);

        if(curNode == NULL)
        {
            return NULL;
        }
    } while(!ptNodeIsExecutable(*curNode));

    return curNode;

}
PTNode* findFirstPTChildWithType(PTNode node, PTType type)
{

    PTNode* ret = NULL;

    iteratePTNodeChildrenForward(node, curNode)
    {
        if(curNode->Type == type)
        {
            ret = curNode;
            break;
        }
    }

    return ret;

}
PTNode* findLastPTChildWithType(PTNode node, PTType type)
{

    PTNode* ret = NULL;

    iteratePTNodeChildrenBackward(node, curNode)
    {
        if(curNode->Type == type)
        {
            ret = curNode;
            break;
        }
    }

    return ret;

}


const char* ptTypeToCString(PTType type)
{

    switch(type)
    {

        case PRoot: return "Root";
        case PIgnorable: return "Ignorable";
        case PDef: return "Def";
        case PDefHeader: return "DefHeader";
        case PValueListStart: return "ValueListStart";
        case PValueListEnd: return "ValueListEnd";
        case PUnlabeledValueList: return "UnlabeledValueList";
        case PJumpListStart: return "JumpListStart";
        case PJumpListEnd: return "JumpListEnd";
        case PUnlabeledJumpList: return "UnlabeledJumpList";
        case PSeparator: return "Separator";
        case PDefStart: return "DefStart";
        case PDefEnd: return "DefEnd";
        case PDefBody: return "DefBody";
        case PLabel: return "Label";
        case PLabelMarker: return "LabelMarker";
        case PSpec: return "Spec";
        case PName: return "Name";
        case PLabeledValueList: return "LabeledValueList";
        case PLabeledJumpList: return "LabeledJumpList";
        case PSpecTerm: return "SpecTerm";

        default: return "Unknown";

    }

}


void printPTNode(PTNode* node, FILE* stream)
{

    assert(node != NULL);

    for(size_t i = 0; i < node->Depth; ++i)
    {
        fprintf(stream, "\t");
    }

    fprintf(stream, "%s:%zu:%zu-%zu:%zu(",
            ptTypeToCString(node->Type), node->startLine, node->startColumn,
            node->endLine, node->endColumn);
    writeString(node->Data, stdout);
    fprintf(stream, ")\n");

    iteratePTNodeChildrenForward(*node, curNode)
    {
        printPTNode(curNode, stream);
    }

}
void printPT(PT* pt, FILE* stream)
{

    assert(pt != NULL);

    fprintf(stream, "Number of nodes: %zd, Height: %zd, Maximum degree: %zd\n",
            pt->Tree.Count, pt->Height, pt->maxDegree);
    fprintf(stream, "Nodes:\n");
    printPTNode((PTNode*)pt->Tree.Root, stream);

}


void finalizePTNode(PTNode* node)
{

    assert(node != NULL);
    ListNode* curNode = node->Header.Children.First;
    ListNode* lastNode = NULL;
    while(curNode != NULL)
    {
        finalizePTNode((PTNode*)curNode);
        lastNode = curNode;
        curNode = curNode->Next;
        finalizeString(&((PTNode*)lastNode)->Data);
        free(lastNode);
    }

}
void finalizePT(PT* pt)
{

    finalizePTNode((PTNode*)pt->Tree.Root);
    free(pt->Tree.Root);
    *pt = (struct PT){0};

}

