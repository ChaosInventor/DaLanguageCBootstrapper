#include <assert.h>
#include "DaLanguageCBootstrap/List.h"
#include "DaLanguageCBootstrap/Tree.h"

void insertTreeNodeAfterTreeNode(TreeNode* toInsert, TreeNode* after)
{

    assert(toInsert != NULL);
    assert(after != NULL);
    assert(after->Parent != NULL);

    toInsert->Parent = after->Parent;
    listInsertNext(&toInsert->Header, &after->Header, &after->Parent->Children);

}
void insertTreeNodeBeforeTreeNode(TreeNode* toInsert, TreeNode* before)
{

    assert(toInsert != NULL);
    assert(before != NULL);
    assert(before->Parent != NULL);

    toInsert->Parent = before->Parent;
    listInsertPrev(&toInsert->Header, &before->Header, &before->Parent->Children);

}
void insertTreeNodeAfterIndexInNode(TreeNode* toInsert, size_t index, TreeNode* inNode)
{

    assert(toInsert != NULL);
    assert(inNode != NULL);
    assert(inNode->Parent != NULL);

    toInsert->Parent = inNode;
    listInsertAfterIndex(&toInsert->Header, index, &inNode->Children);

}
void insertTreeNodeBeforeIndexInNode(TreeNode* toInsert, size_t index, TreeNode* inNode)
{

    assert(toInsert != NULL);
    assert(inNode != NULL);
    assert(inNode->Parent != NULL);

    toInsert->Parent = inNode;
    listInsertBeforeIndex(&toInsert->Header, index, &inNode->Children);

}

void insertTreeNodeAfterTreeNodeTree(TreeNode* toInsert, TreeNode* after, Tree* tree)
{

    assert(tree != NULL);

    insertTreeNodeAfterTreeNode(toInsert, after);
    ++tree->Count;

}
void insertTreeNodeBeforeTreeNodeTree(TreeNode* toInsert, TreeNode* before, Tree* tree)
{

    assert(tree != NULL);

    insertTreeNodeBeforeTreeNode(toInsert, before);
    ++tree->Count;

}
void insertTreeNodeAfterIndexInNodeTree(TreeNode* toInsert, size_t index, TreeNode* inNode, Tree* tree)
{

    assert(tree != NULL);

    insertTreeNodeAfterIndexInNode(toInsert, index, inNode);
    ++tree->Count;

}
void insertTreeNodeBeforeIndexInNodeTree(TreeNode* toInsert, size_t index, TreeNode* inNode, Tree* tree)
{

    assert(tree != NULL);

    insertTreeNodeBeforeIndexInNode(toInsert, index, inNode);
    ++tree->Count;

}


void pushTreeNodeToFrontInNode(TreeNode* toPush, TreeNode* inNode)
{

    assert(toPush != NULL);
    assert(inNode != NULL);

    listPushFrontNode(&toPush->Header, &inNode->Children);
    toPush->Parent = inNode;

}
void pushTreeNodeToBackInNode(TreeNode* toPush, TreeNode* inNode)
{

    assert(toPush != NULL);
    assert(inNode != NULL);

    listPushBackNode(&toPush->Header, &inNode->Children);
    toPush->Parent = inNode;

}

void pushTreeNodeToFrontInNodeTree(TreeNode* toPush, TreeNode* inNode, Tree* tree)
{

    assert(tree != NULL);

    pushTreeNodeToFrontInNode(toPush, inNode);
    ++tree->Count;

}
void pushTreeNodeToBackInNodeTree(TreeNode* toPush, TreeNode* inNode, Tree* tree)
{

    assert(tree != NULL);

    pushTreeNodeToBackInNode(toPush, inNode);
    ++tree->Count;

}


TreeNode* extractTreeNodeAfterNode(TreeNode* node)
{

    assert(node != NULL);
    assert(node->Parent != NULL);

    return (TreeNode*)listExtractNext(&node->Header, &node->Parent->Children);

}
TreeNode* extractTreeNodeBeforeNode(TreeNode* node)
{

    assert(node != NULL);
    assert(node->Parent != NULL);

    return (TreeNode*)listExtractPrev(&node->Header, &node->Parent->Children);

}
TreeNode* extractTreeNodeAfterIndexInNode(size_t index, TreeNode* inNode)
{

    assert(inNode != NULL);
    assert(inNode->Parent != NULL);

    return (TreeNode*)listExtractAfterIndex(index, &inNode->Parent->Children);

}
TreeNode* extractTreeNodeBeforeIndexInNode(size_t index, TreeNode* inNode)
{

    assert(inNode != NULL);
    assert(inNode->Parent != NULL);

    return (TreeNode*)listExtractBeforeIndex(index, &inNode->Parent->Children);

}

TreeNode* extractTreeNodeAfterNodeTree(TreeNode* node, Tree* tree)
{

    assert(tree != NULL);

    --tree->Count;
    return extractTreeNodeAfterNode(node);

}
TreeNode* extractTreeNodeBeforeNodeTree(TreeNode* node, Tree* tree)
{

    assert(tree != NULL);

    --tree->Count;
    return extractTreeNodeBeforeNode(node);

}
TreeNode* extractTreeNodeAfterIndexInNodeTree(size_t index, TreeNode* inNode, Tree* tree)
{

    assert(tree != NULL);

    --tree->Count;
    return extractTreeNodeAfterIndexInNode(index, inNode);

}
TreeNode* extractTreeNodeBeforeIndexInNodeTree(size_t index, TreeNode* inNode, Tree* tree)
{

    assert(tree != NULL);

    --tree->Count;
    return extractTreeNodeBeforeIndexInNode(index, inNode);

}


TreeNode* popFrontTreeNode(TreeNode* node)
{

    assert(node != NULL);

    return (TreeNode*)listPopFrontNode(&node->Children);

}
TreeNode* popBackTreeNode(TreeNode* node)
{

    assert(node != NULL);

    return (TreeNode*)listPopBackNode(&node->Children);

}

TreeNode* popFrontTreeNodeTree(TreeNode* node, Tree* tree)
{

    assert(tree != NULL);

    --tree->Count;
    return popFrontTreeNode(node);

}
TreeNode* popBackTreeNodeTree(TreeNode* node, Tree* tree)
{

    assert(tree != NULL);

    --tree->Count;
    return popBackTreeNode(node);

}


void treeReParent(TreeNode* node, TreeNode* newParent)
{

    assert(node != NULL);
    assert(newParent != NULL);

    if(node->Parent != NULL)
    {
        if(node->Header.Next != NULL)
        {
            listExtractPrev(node->Header.Next, &node->Parent->Children);
        }
        else if(node->Header.Prev != NULL)
        {
            listExtractNext(node->Header.Prev, &node->Parent->Children);
        }
        else
        {
            listPopFrontNode(&node->Parent->Children);
        }

        node->Header.Next = NULL;
        node->Header.Prev = NULL;
    }

    listPushBackNode(&node->Header, &newParent->Children);
    node->Parent = newParent;

}


TreeNode* treeRoot(TreeNode* node)
{

    assert(node != NULL);

    while(node->Parent != NULL)
    {
        node = node->Parent;
    } 

    return node;

}


size_t countTreeNodes(TreeNode* node)
{

    assert(node != NULL);

    size_t ret = 0;

    iterateTreeNodeChildrenForward(*node, curNode)
    {
        if(curNode->Children.Count != 0)
        {
            ret += countTreeNodes(curNode);
        }
        else
        {
            ret += 1;
        }
    }

    return ret + 1;

}
Tree inferTree(TreeNode* node)
{

    Tree ret = {0};

    ret.Root = treeRoot(node);
    ret.Count = countTreeNodes(ret.Root);

    return ret;

}

