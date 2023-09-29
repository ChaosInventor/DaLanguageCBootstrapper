#ifndef DALCBOOT_PARSER
#define DALCBOOT_PARSER

#include "DaLanguageCBootstrap/Tokenizer.h"
#include "DaLanguageCBootstrap/PT.h"

typedef struct ParserContext
{
    
    PT PT;
    PTNode* curNode;
    const char* errorMsg;

} ParserContext;

ParserContext initializeParser(void);

void parseToken(ParserContext* context, Token token);

void finalizeParser(ParserContext* context);

#endif //DALCBOOT_PARSER

