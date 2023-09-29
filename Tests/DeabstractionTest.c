#include <stdio.h>
#include <stdlib.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Parser.h"
#include "DaLanguageCBootstrap/AST.h"
#include "DaLanguageCBootstrap/Unparser.h"
#include "DaLanguageCBootstrap/Detokenizer.h"

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

    printAbstractDef(def, stderr);

    PT pt = initializePTWithRoot();

    PTNode* defNode = allocatePTNode();

    initializePTNodeFromAbstractDefAt(defNode, def, &pt, (PTNode*)pt.Tree.Root);

    PTNode* node = (PTNode*)pt.Tree.Root;
    do
    {
        finalizeToken(&curToken);
        curToken = unparseToken(&node);
        detokenize(curToken, stdout);
    } while(curToken.Type != End);

    finalizePT(&pt);
    finalizeAbstractDef(&def);
    finalizeTokenizer(&tokenizer);
    finalizeParser(&parser);

    return 0;

}

