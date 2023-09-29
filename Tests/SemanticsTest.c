#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Parser.h"
#include "DaLanguageCBootstrap/AST.h"
#include "DaLanguageCBootstrap/SemanticAnalyzer.h"

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

    finalizeTokenizer(&tokenizer);

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

    finalizeParser(&parser);

    SemanticDef topDef;
    SemanticAnalysisResult result = initializeSemanticDefFromAbstractDefAt(&topDef, def);

    if(result.Errors.Count > 0)
    {
        printf("Could not initialize semantic def from abstract def, errors in analysis result.\n");
        printSemanticAnalysisResult(result, stdout);
        finalizeSemanticAnalysisResult(&result);
        return -1;
    }

    finalizeAbstractDef(&def);

    dumpSemanticDef(topDef, stdout);

    finalizeSemanticDef(&topDef);

    return 0;

}

