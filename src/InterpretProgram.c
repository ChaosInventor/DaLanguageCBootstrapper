#include "DaLanguageCBootstrap/InterpretProgram.h"
#include <assert.h>
#include <stdlib.h>

ProgramResult interpretProgram(const char* program, SymbolTable symbols)
{

    assert(program != NULL);

    ProgramResult ret = {0};

    FILE* programStream = fopen(program, "r");
    if(programStream == NULL)
    {
        ret.Type = PREFile;
        ret.Data.PREFile.Program = program;
        ret.Data.PREFile.Perms = "r";

        return ret;
    }


    TokenizerContext tokenizer = initializeTokenizer(programStream);
    ParserContext parser = initializeParser();

    Token curToken = {0};
    do
    {
        finalizeToken(&curToken);
        curToken = nextToken(&tokenizer);

        parseToken(&parser, curToken);
        if(parser.errorMsg != NULL)
        {
            ret.Type = PREParser;
            ret.Data.PREParser.Parser = parser;
            ret.Data.PREParser.errorToken = curToken;

            finalizeTokenizer(&tokenizer);
            fclose(programStream);

            return ret;
        }
    } while(curToken.Type != End);

    finalizeToken(&curToken);
    finalizeTokenizer(&tokenizer);

    fclose(programStream);


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
        ret.Type = PRENoDef;
        ret.Data.PRENoDef.Parser = parser;

        return ret;
    }

    AbstractDef def = initializeAbstractDefFromPTNode(*firstDef);

    finalizeParser(&parser);


    SemanticDef topDef;
    SemanticAnalysisResult result = initializeSemanticDefFromAbstractDefAt(&topDef, def);
    if(result.Errors.Count > 0)
    {
        ret.Type = PRESemantic;
        ret.Data.PRESemantic.Result = result;

        finalizeAbstractDef(&def);

        return ret;
    }

    finalizeAbstractDef(&def);


    InterpreterContext interpreter = initializeInterpreter(symbols, &topDef);
    if(interpreter.Result.Type != IRunning)
    {
        ret.Type = PREInterpretation;
        ret.Data.PREInterpretation.Interpreter = interpreter;

        finalizeSemanticDef(&topDef);

        return ret;
    }

    runInterpreter(&interpreter);

    if(interpreter.Result.Type != ISuccessful)
    {
        ret.Type = PREInterpretation;
        ret.Data.PREInterpretation.Interpreter = interpreter;

        finalizeSemanticDef(&topDef);

        return ret;
    }

    ret.Type = PRSuccessful;
    ret.Data.PRSuccessful.jumpResult = cloneString(interpreter.Result.Data.ISuccessful.Jump);

    finalizeInterpreter(&interpreter);
    finalizeSemanticDef(&topDef);

    return ret;

}

void printProgramResult(
    ProgramResult result, FILE* stream, const char* interpreterName,
    const char* program
)
{

    fprintf(stream, "%s: ", interpreterName);

    switch(result.Type)
    {
        case PRSuccessful:
            {
                fprintf(stream,
                        "Successfully interpreted program %s with resulting "
                        "jump `", program
                       );
                writeString(result.Data.PRSuccessful.jumpResult, stream);
                fprintf(stream, "`\n");
            } break;
        case PREFile:
            {
                fprintf(stream,
                        "Could not open program %s with permissions `%s`\n",
                        result.Data.PREFile.Program, result.Data.PREFile.Perms
                       );
            }break;
        case PREParser:
            {
                fprintf(stream,
                        "Parser error: `%s` on token at %zi:%zi\n",
                        result.Data.PREParser.Parser.errorMsg,
                        result.Data.PREParser.errorToken.startLine,
                        result.Data.PREParser.errorToken.startColumn
                       );
                fprintf(stream,
                        "%s: Dumping token and parse tree...\n",
                        interpreterName
                       );
                printToken(result.Data.PREParser.errorToken, stream);
                printPT(&result.Data.PREParser.Parser.PT, stream);
            } break;
        case PRENoDef:
            {
                fputs(
                      "No top instruction definition found, dumping parse tree."
                      "\n", stream
                     );
                printPT(&result.Data.PRENoDef.Parser.PT, stream);
            } break;
        case PRESemantic:
            {
                fprintf(stream, "Semantic analysis errors, dumping...\n");
                printSemanticAnalysisResult(result.Data.PRESemantic.Result, stream);
            } break;
        case PREInterpretation:
            {
                fprintf(stream, "Interpretation error, dumping...\n");
                assert(result.Data.PREInterpretation.Interpreter.Result.Type == IESemantic);
                printSemanticAnalysisResult(
                    result.Data.PREInterpretation.Interpreter.Result.Data.IESemantic.semanticsResult,
                    stream
                );
            } break;
        default:
            {
                fprintf(stderr, "Unknown program result type, cannot finalize, aborting...\n");
                abort();
            } break;
    }

}

void finalizeProgramResult(ProgramResult* result)
{

    assert(result != NULL);

    switch(result->Type)
    {
        case PRSuccessful:
            {
                finalizeString(&result->Data.PRSuccessful.jumpResult);
            } break;
        case PREFile: break;
        case PREParser:
            {
                finalizeParser(&result->Data.PREParser.Parser);
                finalizeToken(&result->Data.PREParser.errorToken);
            } break;
        case PRENoDef:
            {
                finalizeParser(&result->Data.PRENoDef.Parser);
            } break;
        case PRESemantic:
            {
                finalizeSemanticAnalysisResult(&result->Data.PRESemantic.Result);
            } break;
        case PREInterpretation:
            {
                finalizeInterpreter(&result->Data.PREInterpretation.Interpreter);
            } break;
        default:
            {
                fprintf(stderr, "Unknown program result type, cannot finalize, aborting...\n");
                abort();
            } break;
    }

    *result = (struct ProgramResult){0};

}

void destroyProgramResult(ProgramResult* result)
{
    finalizeProgramResult(result);
    free(result);
}

