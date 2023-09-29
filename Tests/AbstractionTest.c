#include <stdio.h>
#include <stdlib.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Parser.h"
#include "DaLanguageCBootstrap/AST.h"

int main()
{

    TokenizerContext tokenizer = initializeTokenizer(stdin);
    ParserContext parser = initializeParser();

    Token curToken = {0};
    do
    {
        finalizeToken(&curToken);
        curToken = nextToken(&tokenizer);

        parseToken(&parser, curToken);
        if(parser.curNode == NULL && curToken.Type != End)
        {
            printf("Parser state is error, halting\n");
            abort();
        }
    } while(curToken.Type != End);
    finalizeToken(&curToken);

    PTNode* root = (PTNode*)parser.PT.Tree.Root;

    PTNode* firstDef = NULL;
    iteratePTNodeChildrenForward(*root, curNode)
    {
        if(curNode->Type == PDef)
        {
            firstDef = curNode;
            break;
        }
    }

    if(firstDef == NULL)
    {
        printf("Error, could not find a def under the root.");
    }

    AbstractDef def = initializeAbstractDefFromPTNode(*firstDef);

    printAbstractDef(def, stdout);

    finalizeAbstractDef(&def);
    finalizeParser(&parser);
    finalizeTokenizer(&tokenizer);

    return 0;

}

