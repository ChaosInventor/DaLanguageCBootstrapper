#include <stdio.h>
#include "DaLanguageCBootstrap/Tokenizer.h"

int main()
{

    TokenizerContext cont = initializeTokenizer(stdin);

    Token curToken = {0};
    do
    {
        finalizeToken(&curToken);
        curToken = nextToken(&cont);
        printToken(curToken, stdout);
        printf("\n");
    } while(curToken.Type != End);

    finalizeToken(&curToken);

    finalizeTokenizer(&cont);

    return 0;

}

