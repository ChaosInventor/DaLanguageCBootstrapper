#include <stdio.h>
#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/Detokenizer.h"

int main()
{

    TokenizerContext cont = initializeTokenizer(stdin);

    Token curToken = {0};
    do
    {
        finalizeToken(&curToken);

        curToken = nextToken(&cont);

        detokenize(curToken, stdout);
    } while(curToken.Type != End);
    finalizeToken(&curToken);

    finalizeTokenizer(&cont);

    return 0;

}

