#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Parser.h"

PT* pt;

void printOnInt(int sig)
{
    (void)sig;
    printPT(pt, stdout);
    exit(-1);
}

int main()
{

    TokenizerContext tokenizer = initializeTokenizer(stdin);
    ParserContext parser = initializeParser();

    pt = &parser.PT; //Needed for printing from signal
    signal(SIGINT, printOnInt);

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

    printPT(&parser.PT, stdout);

    finalizeTokenizer(&tokenizer);
    finalizeParser(&parser);

    return 0;

}

