#include "DaLanguageCBootstrap/Detokenizer.h"
#include <assert.h>

void detokenize(Token token, FILE* stream)
{
    if(token.Type != End) writeString(token.Data, stream);
}

