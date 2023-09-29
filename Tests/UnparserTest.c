#include <stdio.h>
#include <stdlib.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Detokenizer.h"
#include "DaLanguageCBootstrap/Parser.h"
#include "DaLanguageCBootstrap/Unparser.h"

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

    PTNode* node = (PTNode*)parser.PT.Tree.Root;
    do
    {
        finalizeToken(&curToken);
        curToken = unparseToken(&node);
        detokenize(curToken, stdout);
    } while(curToken.Type != End);

    finalizeTokenizer(&tokenizer);
    finalizeParser(&parser);

    return 0;

}

