#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "DaLanguageCBootstrap/Tokenizer.h"

static const size_t DEFAULT_TOKENIZER_BUFFER_SIZE = 1024 * 1024 * 2; //2MiB

static bool nextChar(TokenizerContext* context)
{

    assert(context != NULL);

    //TODO: Move to a more generic string iterator function
    context->curIndex++;
    while(context->curIndex >= context->curNode->UsedUpper)
    {
        advanceStringNode(context->curNode);

        if(stringNodeIsTerminal(context->curNode))
        {
            emptyString(&context->Buffer);
            size_t red = readStringStart(&context->Buffer, context->Stream);
            context->curNode = getFirstStringNode(context->Buffer);
            context->curIndex = getFirstIndexStringNode(*context->curNode);

            if(red == 0)
            {
                context->curChar = 0;
                return false;
            }
        }
        else context->curIndex = getFirstIndexStringNode(*context->curNode);
    }

    //The new line character itself is considered to be the last char of a line.
    //Note: here curChar is technically the previous char.
    if(context->curChar == '\n')
    {
        context->curLine += 1;
        context->curColumn = 1;
    }
    else context->curColumn +=1;
    context->curChar = context->curNode->Buffer[context->curIndex];

    return true;

}
static void pushData(Token* token, TokenizerContext* context)
{

    pushCharToString(context->curChar, &token->Data);

    token->endLine = context->curLine;
    token->endColumn = context->curColumn;

}

TokenizerContext initializeTokenizerSize(FILE* stream, size_t bufferSize)
{

    assert(bufferSize > 0);

    TokenizerContext ret = {0};

    ret.Stream = stream;
    ret.Buffer = initializeString(bufferSize);

    readStringStart(&ret.Buffer, ret.Stream);

    ret.curNode = getFirstStringNode(ret.Buffer);
    ret.curIndex = getFirstIndexStringNode(*ret.curNode);

    ret.curLine = 1;
    ret.curColumn = 1;
    if(ret.curIndex < ret.curNode->UsedUpper) ret.curChar = ret.curNode->Buffer[ret.curIndex];
    else ret.curChar = 0;

    return ret;

}
TokenizerContext initializeTokenizer(FILE* stream)
{
    return initializeTokenizerSize(stream, DEFAULT_TOKENIZER_BUFFER_SIZE);
}

Token nextToken(TokenizerContext* context)
{

    assert(context != NULL);

    Token ret = {0};

    ret.Type = End;

    ret.startColumn = context->curColumn;
    ret.startLine = context->curLine;
    pushData(&ret, context);

    //Needed to compute the token type.
    char lastChar = context->curChar;

    bool eof = !nextChar(context);
    if(eof && lastChar == '\0')
    {
        //EOF, return an End token with '\0' as data.
        return ret;
    }
    else
    {
        if(isIgnorable(lastChar))
        {
            ret.Type = Ignorable;

            //Make sure not to push in the null char at EOF
            while(isIgnorable(context->curChar) && !eof)
            {
                pushData(&ret, context);
                if(!nextChar(context)) break;
            }
        }
        else
        {
            switch(lastChar)
            {
                case '(': ret.Type = ValueListStart; break;
                case ')': ret.Type = ValueListEnd; break;
                case '{': ret.Type = DefStart; break;
                case '}': ret.Type = DefEnd; break;
                case '[': ret.Type = JumpListStart; break;
                case ']': ret.Type = JumpListEnd; break;
                case ';': ret.Type = SpecTerm; break;
                case ',': ret.Type = Separator; break;
                case ':': ret.Type = LabelMarker; break;

                default:
                {
                    ret.Type = Name;
                    while(isLetter(context->curChar))
                    {
                        pushData(&ret, context);
                        if(!nextChar(context)) break;
                    }
                } break;
            }
        }
    }

    return ret;

}

bool isLetter(char c)
{

    return
        (c >= 0x30 && c <= 0x39) //Numbers, 0-9
        ||
        (c >= 0x41 && c <= 0x5A) //Upper case letters, A-Z
        ||
        (c >= 0x61 && c <= 0x7A) //Lower case letters, a-z
        ;

}
bool isIgnorable(char c)
{

    switch(c)
    {
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ';':
        case ',':
        case ':':
        case '$':
        case '<':
        case '>':
        case '.':
        case '?':
        case '&':
        case '|':
        case '~':
        case '!':
        case '+':
        case '-':
        case '%':
        case '^':
            return false;
        default: return !isLetter(c);
    }

}

const char* tokenTypeToCString(TokenType type)
{

    switch(type)
    {
        case End: return "End";
        case Ignorable: return "Ignorable";
        case Name: return "Name";
        case LabelMarker: return "LabelMarker";
        case Separator: return "Separator";
        case ValueListStart: return "ValueListStart";
        case ValueListEnd: return "ValueListEnd";
        case JumpListStart: return "JumpListStart";
        case JumpListEnd: return "JumpListEnd";
        case DefStart: return "DefStart";
        case DefEnd: return "DefEnd";
        case SpecTerm: return "SpecTerm";

        default: return "Unknown";
    }

}

void printToken(Token token, FILE* stream)
{

    fprintf(stream, "\x1B[35m%s:%zu:%zu-%zu:%zu\x1B[0m",
            tokenTypeToCString(token.Type), token.startLine, token.startColumn,
            token.endLine, token.endColumn
        );
    fprintf(stream, "\x1B[32m(\x1B[0m");
    writeString(token.Data, stdout);
    fprintf(stream, "\x1B[32m)\x1B[0m");

}

void finalizeToken(Token* token)
{

    finalizeString(&token->Data);
    *token = (struct Token){0};

}
void finalizeTokenizer(TokenizerContext* context)
{

    assert(context != NULL);

    finalizeString(&context->Buffer);
    *context = (TokenizerContext){0};

}

