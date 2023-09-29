#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "DaLanguageCBootstrap/Parser.h"

#define UNEXPECTED_TOKEN() { parserError("Unexpected token in current state", context, token); }

#define curType() context->curNode->Type
#define Ascend()  PTNodeAscend(context->curNode)
#define Descend()  PTNodeDescend(context->curNode)
#define newChild(type)\
    context->curNode = addChild(type)
#define addChild(type)\
    createPTNodeFromToken(context->curNode, type, &context->PT, token);
#define raiseError(msg)\
    { parserError(msg, context, token); return; }

ParserContext initializeParser()
{

    ParserContext ret = {0};

    PTNode* root = malloc(sizeof(PTNode));
    assert(root != NULL);
    *root = (struct PTNode){0};
    root->Header.Children = (struct List){0};
    root->Header.Parent = NULL;
    root->Type = PRoot;
    root->Data = (struct String){0};
    root->startLine = 1;
    root->startColumn = 1;
    root->endLine = 1;
    root->endColumn = 1;
    root->Depth = 0;

    ret.PT.Tree.Root = (TreeNode*)root;
    ret.PT.Tree.Count = 1;
    ret.PT.Height = 0;
    ret.PT.maxDegree = 0;
    ret.curNode = root;

    return ret;

}

PTNode* createPTNodeFromToken(PTNode* root, PTType type, PT* pt, Token token)
{

    assert(root != NULL);

    PTNode* ret = allocatePTNode();

    ret->Type = type;
    ret->Data = cloneString(token.Data);

    ret->startLine = token.startLine;
    ret->startColumn = token.startColumn;
    ret->endLine = token.endLine;
    ret->endColumn = token.endColumn;

    pushPTNodeToFront(ret, root, pt);

    return ret;

}
void parserError(const char* msg, ParserContext* context, Token token)
{

    assert(msg != NULL);
    assert(context != NULL);

    (void)token;

    //TODO: Potentially use the token to add line and column information to the
    //error message?

    context->curNode = NULL;
    context->errorMsg = msg;

}
void parseToken(ParserContext* context, Token token)
{

    assert(context != NULL);

    if(context->curNode == NULL)
    {
        return;
    }

    bool done = false;
    while (!done)
    {
        switch(context->curNode->Type)
        {

            case PRoot: switch(token.Type)
                        {

                            case Ignorable: addChild(PIgnorable); done = true; break;
                            case ValueListStart: newChild(PDef); break;
                            case End:
                                {
                                    context->curNode->endLine = token.endLine;
                                    context->curNode->endColumn = token.endColumn;
                                    context->curNode = NULL;
                                    done = true;
                                } return;

                            default: UNEXPECTED_TOKEN(); return;

                        } break;

            case PDef: switch(token.Type)
                       {

                           case ValueListStart: newChild(PDefHeader); break;
                           case DefStart: newChild(PDefBody); break;
                           case DefEnd:
                               {
                                   context->curNode->endLine = PTNodeLastChild(*context->curNode)->endLine;
                                   context->curNode->endColumn = PTNodeLastChild(*context->curNode)->endColumn;
                                   Ascend();
                                   done = true;
                               } break;

                           default: UNEXPECTED_TOKEN(); break;
                       } break;
            case PDefHeader: switch(token.Type)
                             {

                                 case ValueListStart:
                                     {
                                        if(ptNodeHasChildOfType(*context->curNode, PUnlabeledValueList))
                                            raiseError("Definition header cannot have multiple value lists");

                                        newChild(PUnlabeledValueList);
                                     } break;
                                 case JumpListStart:
                                     {
                                         if(ptNodeHasChildOfType(*context->curNode, PUnlabeledJumpList))
                                             raiseError("Definition header can only have one jump list");
                                         if(!ptNodeHasChildOfType(*context->curNode, PUnlabeledValueList))
                                             raiseError("Jump list must come after a value list in a definition header");

                                         newChild(PUnlabeledJumpList);
                                     } break;
                                 case Ignorable: addChild(PIgnorable); done = true; break;

                                 case DefStart:
                                     {
                                         if(PTNodeFirstChild(*context->curNode)->Type != PUnlabeledValueList)
                                             raiseError("Cannot have a definition without an unlabeled value list");

                                         context->curNode->endLine = PTNodeLastChild(*context->curNode)->endLine;
                                         context->curNode->endColumn = PTNodeLastChild(*context->curNode)->endColumn;
                                         Ascend();
                                     } break;

                                 default:
                                     {
                                         if(findFirstPTChildWithType(*context->curNode, PUnlabeledValueList) != NULL)
                                             Ascend();
                                         else UNEXPECTED_TOKEN();
                                     } break;

                             } break;
            case PDefBody: switch(token.Type)
                           {

                               case Name: newChild(PName); done = true; break;
                               case DefStart:
                                   {
                                       addChild(PDefStart);
                                       done = true;
                                   } break;
                               case DefEnd:
                                   {
                                       bool hasSpec = false;
                                       iteratePTNodeChildrenForward(*context->curNode, curNode)
                                       {
                                           PTNode* inspect = curNode;
                                           if(inspect->Type == PLabel)
                                           {
                                               iteratePTNodeChildrenForward(*inspect, node)
                                               {
                                                   if(node->Type == PSpec)
                                                   {
                                                       inspect = node;
                                                       break;
                                                   }
                                               }
                                           }

                                           if(inspect->Type == PSpec)
                                           {
                                               hasSpec = true;
                                               break;
                                           }
                                       }

                                       if(hasSpec)
                                       {
                                           addChild(PDefEnd);
                                           Ascend();
                                       }
                                       else
                                       {
                                           raiseError("Definition body is being terminated but it does not have any specs");
                                       }
                                   } break;
                               case Ignorable: addChild(PIgnorable); done = true; break;

                               default: UNEXPECTED_TOKEN(); return;

                           } break;

            case PLabel: switch(token.Type)
                         {
                             case Name:
                                 {
                                     bool canAdd = true;
                                     PTNode* start = findFirstPTChildWithType(*context->curNode, PLabelMarker);
                                     PTNodeAdvance(start);
                                     iteratePTNodeForward(start, node)
                                     {
                                         if(node->Type != PIgnorable)
                                         {
                                             Ascend();
                                             canAdd = false;
                                             break;
                                         }
                                     }

                                     if(canAdd)
                                     {
                                         if(PTNodeParent(*context->curNode)->Type == PDefBody)
                                         {
                                             newChild(PSpec);
                                         }
                                         else
                                         {
                                             newChild(PName);
                                             done = true;
                                         }
                                     }
                                 } break;
                             case ValueListStart:
                                 {
                                     bool canAdd = true;

                                     PTNode* start = findFirstPTChildWithType(*context->curNode, PLabelMarker);
                                     PTNodeAdvance(start);
                                     iteratePTNodeForward(start, node)
                                     {
                                         if(node->Type != PIgnorable)
                                         {
                                             Ascend();
                                             canAdd = false;
                                             break;
                                         }
                                     }

                                     if(canAdd)
                                     {
                                         newChild(PDef);
                                     }
                                     else
                                     {
                                         Ascend();
                                     }
                                 } break;
                             case Ignorable:
                                 {
                                     addChild(PIgnorable);
                                     done = true;
                                 } break;
                             default: Ascend(); break;
                         } break;
            case PName: switch(token.Type)
                        {
                            case Ignorable: addChild(PIgnorable); done = true; break;
                            case LabelMarker:
                                            {
                                                if(PTNodeParent(*context->curNode)->Type ==  PLabel)
                                                    raiseError("Labels cannot be nested");

                                                context->curNode->Type = PLabel;

                                                PTNode* name = allocatePTNode();
                                                name->Type = PName;
                                                name->Data = cloneString(context->curNode->Data);
                                                name->startLine = context->curNode->startLine;
                                                name->startColumn = context->curNode->startColumn;
                                                name->endLine = context->curNode->endLine;
                                                name->endColumn = context->curNode->endColumn;

                                                pushPTNodeToBack(name, context->curNode, &context->PT);

                                                addChild(PLabelMarker);

                                                done = true;
                                            } break;
                            case ValueListStart:
                            case JumpListStart:
                            case SpecTerm:
                                            {
                                                context->curNode->Type = PSpec;

                                                PTNode* name = allocatePTNode();
                                                name->Type = PName;
                                                name->Data = cloneString(context->curNode->Data);
                                                name->startLine = context->curNode->startLine;
                                                name->startColumn = context->curNode->startColumn;
                                                name->endLine = context->curNode->endLine;
                                                name->endColumn = context->curNode->endColumn;

                                                pushPTNodeToBack(name, context->curNode, &context->PT);
                                            } break;
                            default: Ascend();
                        } break;

            case PUnlabeledValueList:
            case PUnlabeledJumpList:
            case PLabeledJumpList:
            case PLabeledValueList: switch(token.Type)
                                    {
                                        case ValueListStart:
                                            {
                                                if(curType() != PLabeledValueList && curType() != PUnlabeledValueList)
                                                    raiseError("Unexpected start of value list inside of jump list");
                                                if(PTNodeChildrenCount(*context->curNode) > 0)
                                                    raiseError("Value lists cannot be nested");

                                                addChild(PValueListStart);
                                                done = true;
                                            } break;
                                        case JumpListStart:
                                            {
                                                if(curType() != PLabeledJumpList && curType() != PUnlabeledJumpList)
                                                    raiseError("Unexpected start of jump list inside of value list");
                                                if(PTNodeChildrenCount(*context->curNode) > 0)
                                                    raiseError("Jump lists cannot be nested");

                                                addChild(PJumpListStart);
                                                done = true;
                                            } break;

                                        case Ignorable:
                                            {
                                                addChild(PIgnorable);
                                                done = true;
                                            } break;
                                        case Separator:
                                        case Name:
                                            {

                                                iteratePTNodeChildrenBackward(*context->curNode, node)
                                                {
                                                    if(node->Type == PSeparator)
                                                    {
                                                        if(token.Type == Separator)
                                                        {
                                                            raiseError("Separator already given");
                                                        }
                                                        else
                                                        {
                                                            break;
                                                        }
                                                    }
                                                    if(node->Type == PLabel || node->Type == PName)
                                                    {
                                                        if(token.Type == Name)
                                                        {
                                                            raiseError("Names not separated using a separator");
                                                        }
                                                        else
                                                        {
                                                            break;
                                                        }
                                                    }
                                                }

                                                newChild(token.Type == Separator? PSeparator : PName);
                                                done = true;
                                            } break;

                                        case ValueListEnd:
                                            {
                                                if(curType() != PLabeledValueList && curType() != PUnlabeledValueList)
                                                    raiseError("Unexpected end of value list inside of jump list");

                                                addChild(PValueListEnd);
                                                context->curNode->endLine = PTNodeLastChild(*context->curNode)->endLine;
                                                context->curNode->endColumn = PTNodeLastChild(*context->curNode)->endColumn;
                                                Ascend();
                                                done = true;
                                            } break;
                                        case JumpListEnd:
                                            {
                                                if(curType() != PLabeledJumpList && curType() != PUnlabeledJumpList)
                                                    raiseError("Unexpected end of jump list inside of value list");

                                                addChild(PJumpListEnd);
                                                context->curNode->endLine = PTNodeLastChild(*context->curNode)->endLine;
                                                context->curNode->endColumn = PTNodeLastChild(*context->curNode)->endColumn;
                                                Ascend();
                                                done = true;
                                            } break;

                                        default: UNEXPECTED_TOKEN(); return;
                                    } break;

            case PSeparator: Ascend(); break;

            case PSpec: switch(token.Type)
                        {

                            case Name:
                                {
                                    iteratePTNodeChildrenForward(*context->curNode, node)
                                    {
                                        if(node->Type != PIgnorable)
                                        {
                                            raiseError("Specification must have a name as its first member");
                                        }
                                    }

                                    addChild(PName);
                                    done = true;
                                } break;
                            case ValueListStart:
                                {
                                    iteratePTNodeChildrenBackward(*context->curNode, node)
                                    {
                                        if(node->Type != PName && node->Type != PIgnorable)
                                        {
                                            raiseError("Value list inside specification must come after name, and there can only be one");
                                        }
                                    }

                                    newChild(PLabeledValueList);
                                } break;
                            case JumpListStart:
                                {
                                    if(
                                            ptNodeHasChildOfType(*context->curNode, PLabeledJumpList)
                                            ||
                                            ptNodeHasChildOfType(*context->curNode, PSpecTerm)
                                            ||
                                            !ptNodeHasChildOfType(*context->curNode, PName)
                                      )
                                    {
                                        raiseError("Jump list in specification must come after value list or name, and there can only be one");
                                    }

                                    newChild(PLabeledJumpList);
                                } break;
                            case SpecTerm:
                                {
                                    addChild(PSpecTerm);
                                    context->curNode->endLine = PTNodeLastChild(*context->curNode)->endLine;
                                    context->curNode->endColumn = PTNodeLastChild(*context->curNode)->endColumn;
                                    Ascend();
                                    done = true;
                                } break;
                            case Ignorable: addChild(PIgnorable); done = true; break;

                            default: UNEXPECTED_TOKEN(); return;

                        } break;

            default: raiseError("Unknown parser state"); break;

        }
    }

}

void finalizeParser(ParserContext* context)
{

    assert(context != NULL);

    finalizePTNode((PTNode*)context->PT.Tree.Root);

    finalizeString(&((PTNode*)context->PT.Tree.Root)->Data);
    free(context->PT.Tree.Root);

}

