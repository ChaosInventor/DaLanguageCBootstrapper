#ifndef DALCBOOT_TREE
#define DALCBOOT_TREE

#include <stddef.h>
#include "DaLanguageCBootstrap/List.h"

#define iterateTreeNodeChildrenForwardType(treeNode, curNode, Type, Next)\
    iterateListForwardType((treeNode).Children, curNode, Type, Next)
#define iterateTreeNodeChildrenBackwardType(treeNode, curNode, Type, Next)\
    iterateListBackwardType((treeNode).Children, curNode, Type, Next)

#define iterateTreeNodeChildrenForward(treeNode, curNode)\
    iterateTreeNodeChildrenForwardType(treeNode, curNode, TreeNode, Header.Next)
#define iterateTreeNodeChildrenBackward(treeNode, curNode)\
    iterateTreeNodeChildrenBackwardType(treeNode, curNode, TreeNode, Header.Next)

typedef struct TreeNode
{

    ListNode Header;
    List Children;
    struct TreeNode* Parent;

} TreeNode;

typedef struct Tree
{

    TreeNode* Root;
    size_t Count;

} Tree;

void insertTreeNodeAfterTreeNode(TreeNode* toInsert, TreeNode* after);
void insertTreeNodeBeforeTreeNode(TreeNode* toInsert, TreeNode* before);
void insertTreeNodeAfterIndexInNode(TreeNode* toInsert, size_t index, TreeNode* inNode);
void insertTreeNodeBeforeIndexInNode(TreeNode* toInsert, size_t index, TreeNode* inNode);

void insertTreeNodeAfterTreeNodeTree(TreeNode* toInsert, TreeNode* after, Tree* tree);
void insertTreeNodeBeforeTreeNodeTree(TreeNode* toInsert, TreeNode* before, Tree* tree);
void insertTreeNodeAfterIndexInNodeTree(TreeNode* toInsert, size_t index, TreeNode* inNode, Tree* tree);
void insertTreeNodeBeforeIndexInNodeTree(TreeNode* toInsert, size_t index, TreeNode* inNode, Tree* tree);


void pushTreeNodeToFrontInNode(TreeNode* toPush, TreeNode* inNode);
void pushTreeNodeToBackInNode(TreeNode* toPush, TreeNode* inNode);

void pushTreeNodeToFrontInNodeTree(TreeNode* toPush, TreeNode* inNode, Tree* tree);
void pushTreeNodeToBackInNodeTree(TreeNode* toPush, TreeNode* inNode, Tree* tree);


TreeNode* extractTreeNodeAfterNode(TreeNode* node);
TreeNode* extractTreeNodeBeforeNode(TreeNode* node);
TreeNode* extractTreeNodeAfterIndexInNode(size_t index, TreeNode* inNode);
TreeNode* extractTreeNodeBeforeIndexInNode(size_t index, TreeNode* inNode);

TreeNode* extractTreeNodeAfterNodeTree(TreeNode* node, Tree* tree);
TreeNode* extractTreeNodeBeforeNodeTree(TreeNode* node, Tree* tree);
TreeNode* extractTreeNodeAfterIndexInNodeTree(size_t index, TreeNode* inNode, Tree* tree);
TreeNode* extractTreeNodeBeforeIndexInNodeTree(size_t index, TreeNode* inNode, Tree* tree);


TreeNode* popFrontTreeNode(TreeNode* node);
TreeNode* popBackTreeNode(TreeNode* node);

TreeNode* popFrontTreeNodeTree(TreeNode* node, Tree* tree);
TreeNode* popBackTreeNodeTree(TreeNode* node, Tree* tree);


void treeReParent(TreeNode* node, TreeNode* newParent);


TreeNode* treeRoot(TreeNode* node);


Tree inferTree(TreeNode* node);

#endif //DALCBOOT_TREE

