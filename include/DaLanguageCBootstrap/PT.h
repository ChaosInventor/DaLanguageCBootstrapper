#ifndef DALCBOOT_PT
#define DALCBOOT_PT

#include "DaLanguageCBootstrap/String.h"
#include "DaLanguageCBootstrap/Tree.h"
#include "DaLanguageCBootstrap/ListOfString.h"
#include "DaLanguageCBootstrap/LabeledList.h"
#include "DaLanguageCBootstrap/Tokenizer.h"

#define PTNodeParent(Node) ((PTNode*)((Node).Header.Parent))
#define PTNodeNext(Node) ((PTNode*)((Node).Header.Header.Next))
#define PTNodePrev(Node) ((PTNode*)((Node).Header.Header.Prev))
#define PTNodeFirstChild(Node) ((PTNode*)((Node).Header.Children.First))
#define PTNodeLastChild(Node) ((PTNode*)((Node).Header.Children.Last))
#define PTNodeChildrenCount(Node) (Node).Header.Children.Count

#define PTNodeAscend(PTNode) (PTNode) = PTNodeParent(*(PTNode))
#define PTNodeDescend(PTNode) (PTNode) = PTNodeFirstChild(*(PTNode))
#define PTNodeDescendEnd(PTNode) (PTNode) = PTNodeFirstChild(*(PTNode))
#define PTNodeAdvance(PTNode) (PTNode) = PTNodeNext(*(PTNode))
#define PTNodeRetreat(PTNode) (PTNode) = PTNodePrev(*(PTNode))

#define iteratePTNodeChildrenForward(Node, curNode) iterateListForwardType((Node).Header.Children, curNode, PTNode, Header.Header.Next) 
#define iteratePTNodeChildrenBackward(Node, curNode) iterateListBackwardType((Node).Header.Children, curNode, PTNode, Header.Header.Prev) 
#define iteratePTNodeForward(startingValue, curNode)\
    iterateListNodeForwardType(curNode, startingValue, PTNode, Header.Header.Next)
#define iteratePTNodeBackward(startingValue, curNode)\
    iterateListNodeBackwardType(curNode, startingValue, PTNode, Header.Header.Next)

#define PTDefHeader(def) PTNodeFirstChild(def)
#define PTDefBody(def) PTNodeLastChild(def)

#define PTDefHeaderValueList(header) PTNodeFirstChild(def)
#define PTDefHeaderJumpList(header) (header).Header.Children.First == (header).Header.Children.Last ? NULL : PTNodeLastChild(header)

#define PTSpecName(spec) PTNodeFirstChild(spec)
#define PTSpecTerm(spec) PTNodeLastChild(spec)

typedef struct PT
{

    Tree Tree;
    size_t Height;
    size_t maxDegree;

} PT;

typedef enum PTType
{

    PRoot,
    PIgnorable,
    PDef,
    PDefHeader,
    PValueListStart,
    PValueListEnd,
    PUnlabeledValueList,
    PJumpListStart,
    PJumpListEnd,
    PUnlabeledJumpList,
    PSeparator,
    PDefStart,
    PDefEnd,
    PDefBody,
    PLabel,
    PLabelMarker,
    PSpec,
    PName,
    PLabeledValueList,
    PLabeledJumpList,
    PSpecTerm,

} PTType;

typedef struct PTNode
{

    TreeNode Header;
    PTType Type;
    String Data;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    size_t Depth;

} PTNode;


PTNode* allocatePTNode(void);


PT initializePTWithRoot(void);

void initializePTNodeFromConstAt(PTNode* ret, const char* data, PTType type, PT* pt, PTNode* root);
void initializePTNameNodeAt(PTNode* ret, String data, PT* pt, PTNode* root);
void initializePTLabelNodeAt(PTNode* ret, String data, PT* pt, PTNode* root);
void initializePTNodeFromListOfStringAt(PTNode* ret, List list, PTType type, PT* pt, PTNode* root);
void initializePTNodeFromLabeledListAt(PTNode* ret, List list, PTType type, PT* pt, PTNode* root);

List initializeListOfStringFromPTNode(PTNode* list);
List initializeLabeledListFromPTNode(PTNode* list);


PTNode* createPTNodeType(PTType type, PT* pt, PTNode* root);


bool ptNodeIsExecutable(PTNode node);
bool ptNodeHasChildOfType(PTNode node, PTType type);


PTNode* findNextNodeInDef(PTNode node);
PTNode* findFirstPTChildWithType(PTNode node, PTType type);
PTNode* findLastPTChildWithType(PTNode node, PTType type);


void pushPTNodeToFront(PTNode* node, PTNode* root, PT* pt);
void pushPTNodeToBack(PTNode* node, PTNode* root, PT* pt);


const char* ptTypeToCString(PTType type);


void printPTNode(PTNode* node, FILE* stream);
void printPT(PT* pt, FILE* stream);


void finalizePTNode(PTNode* node);
void finalizePT(PT* pt);

#endif //DALCBOOT_PT

