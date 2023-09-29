#include <assert.h>
#include <stdbool.h>
#include "DaLanguageCBootstrap/Unparser.h"

#define Ascend() PTNodeAscend(*curNode)
#define Descend() PTNodeDescend(*curNode)
#define Advance() PTNodeAdvance(*curNode)

Token unparseToken(PTNode** curNode)
{

    if(*curNode == NULL)
    {
        return (struct Token){0};
    }

    Token ret = (struct Token){0};
    bool found = false; while(!found)
    {
        switch((*curNode)->Type)
        {
            case PIgnorable: ret.Type = Ignorable; found = true; break;
            case PName: ret.Type = Name; found = true; break;
            case PLabelMarker: ret.Type = LabelMarker; found = true; break;
            case PSeparator: ret.Type = Separator; found = true; break;
            case PValueListStart: ret.Type = ValueListStart; found = true; break;
            case PValueListEnd: ret.Type = ValueListEnd; found = true; break;
            case PJumpListStart: ret.Type = JumpListStart; found = true; break;
            case PJumpListEnd: ret.Type = JumpListEnd; found = true; break;
            case PDefStart: ret.Type = DefStart; found = true; break;
            case PDefEnd: ret.Type = DefEnd; found = true; break;
            case PSpecTerm: ret.Type = SpecTerm; found = true; break;

            default:
                               {
                                   if(PTNodeChildrenCount(**curNode) > 0)
                                   {
                                       Descend();
                                   }
                                   else if(PTNodeNext(**curNode) != NULL)
                                   {
                                       Advance();
                                   }
                                   else
                                   {
                                       while(PTNodeNext(**curNode) == NULL)
                                       {
                                           Ascend();
                                           if(*curNode == NULL)
                                           {
                                               return (struct Token){0};
                                           }
                                       }

                                       Advance();
                                   }
                               } break;
        }
    }

    ret.Data = cloneString((*curNode)->Data);
    ret.startLine = (*curNode)->startLine;
    ret.startColumn = (*curNode)->startColumn;
    ret.endLine = (*curNode)->endLine;
    ret.endColumn = (*curNode)->endColumn;

    if(PTNodeChildrenCount(**curNode) > 0)
    {
        Descend();
    }
    else if(PTNodeNext(**curNode) != NULL)
    {
        Advance();
    }
    else
    {
        while(PTNodeNext(**curNode) == NULL)
        {
            Ascend();
            if(*curNode == NULL)
            {
                return ret;
            }
        }

        Advance();
    }

    return ret;

}

