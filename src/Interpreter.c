#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "DaLanguageCBootstrap/Interpreter.h"
#include "DaLanguageCBootstrap/Instruction.h"

//TODO: Using a stack is inherently inefficient since instructions may not use
//recursion. Collapsing all defs so the top def contains nothing but specs with
//referents to instructions or other specs is far more efficient.
#define INTERPRETER_STACK_STARTING_SIZE 256

InterpreterScope* allocateInterpreterScope()
{

    InterpreterScope* ret = malloc(sizeof(InterpreterScope));
    assert(ret != NULL);

    *ret = (struct InterpreterScope){0};

    return ret;

}

InterpreterScope initializeInterpreterScope(SemanticDef* def)
{

    assert(def != NULL);

    InterpreterScope ret = {.curDef = def, .curSpec = def->executionStart};

    return ret;

}
InterpreterContext initializeInterpreter(SymbolTable startingSymbols, SemanticDef* topDef)
{

    InterpreterContext ret = {0};

    //TODO: The height of the AST could be a good hint as to the stack size.
    ret.Frame = initializeStack(INTERPRETER_STACK_STARTING_SIZE);
    ret.Result.Type = IRunning;

    ret.topDef = topDef;
    SemanticAnalysisResult semResult = resolveSemanticDefSymbols(topDef, startingSymbols);

    if(semResult.Errors.Count > 0)
    {
        finalizeInterpreter(&ret);

        ret.Result.Type = IESemantic;
        ret.Result.Data.IESemantic.semanticsResult = semResult;

        return ret;
    }

    InterpreterScope* scope = malloc(sizeof(InterpreterScope));
    assert(scope != NULL);

    scope->curDef = ret.topDef;
    scope->curSpec = topDef->executionStart;
    scope->Next = NULL;

    stackPush(&ret.Frame, scope);

    return ret;

}

InterpreterScope* createInterpreterScope(SemanticDef* def)
{

    assert(def != NULL);

    InterpreterScope* ret = allocateInterpreterScope();
    *ret = initializeInterpreterScope(def);

    return ret;

}

void resolveNextSpec(InterpreterScope* scope, String nextJump)
{

    //Alias
    SymbolTable jumps = scope->curSpec->Jumps;

    scope->Next = getSymbolTable(jumps, nextJump);
    if(scope->Next == NULL)
    {
        //Alias
        SymbolTable otherJumps = scope->curSpec->executableJumps;

        //Check if the jump has been partially specified
        scope->Next = getSymbolTable(otherJumps, nextJump);
        if(scope->Next == NULL)
        {
            //Find the first jump with a value
            iterateListOfStringForward(otherJumps.symbolsList, jump)
            {
                scope->Next = getSymbolTable(otherJumps, jump->Name);

                //Note: the jump must come from the spec itself, partial
                //specification jumps don't count
                bool owning = false;
                iterateSymbolTableForward(jumps, owningJump)
                {
                    if(owningJump == scope->Next) owning = true;
                }
                iterateListOfPointerForward(jumps.namelessValues, curNode)
                {
                    Symbol* sym = (Symbol*)curNode->Pointer;
                    if(sym == scope->Next) owning = true;
                }

                if(!owning) scope->Next = NULL;

                if(scope->Next != NULL) break;
            }

            //If not jump has a value then default to the next one after the
            //current spec.
            if(scope->Next == NULL)
            {
                ListOfPointer* inList = findFirstPointerInListOfPointer(scope->curSpec, scope->curDef->Specs);
                assert(inList != NULL);
                //Not all specs have symbols, so we can't do a separate jump and
                //execute.
                if(inList->Header.Next != NULL)
                {
                    scope->curSpec = (SemanticSpec*)((ListOfPointer*)inList->Header.Next)->Pointer;
                }
                //The current spec was the last, jump to the containing def's
                //first jump
                else
                {
                    String firstJump = ((ListOfString*)scope->curDef->valuesEnd->Header.Next)->Name;
                    scope->Next = getSymbolTable(scope->curDef->Symbols, firstJump);
                    assert(scope->Next != NULL);
                }
            }
        }
    }

    scope->nextName = nextJump;

}
void runInterpreter(InterpreterContext* context)
{
    while(stepInterpreter(context));
}
bool bigStepInterpreter(InterpreterContext* context)
{

    size_t oldTop = context->Frame.Top;
    InterpreterScope* scope = stackPeek(&context->Frame);

    bool more;
    do more = stepInterpreter(context);
    while(more && (context->Frame.Top > oldTop || scope->Next != NULL));

    return more;

}
bool stepInterpreter(InterpreterContext* context)
{

    if(context->Result.Type != IRunning || context->topDef == NULL)
    {
        return false;
    }

    InterpreterScope* scope = stackPeek(&context->Frame);

    if(scope->Next != NULL)
    {
        if(scope->Next->Type == STSpec)
        {
            SemanticSpec* spec = scope->Next->Data.STSpec;
            if(spec->containingDef != scope->curDef)
            {
                SemanticDef* target = spec->containingDef;

                scope = stackPop(&context->Frame);
                while(scope->curDef != target)
                {
                    destroyInterpreterScope(scope);
                    scope = stackPop(&context->Frame);
                }
                stackPush(&context->Frame, scope);

                assert(scope != NULL);
            }
            scope->curSpec = spec;
            scope->Next = NULL;
        }
        else if(scope->Next->Type == STJump)
        {
            String nextName = scope->Next->Data.STJump.Name;
            SemanticDef* target = scope->Next->Data.STJump.Def;

            if(target == context->topDef)
            {
                context->Result.Type = ISuccessful;
                context->Result.Data.ISuccessful.Jump = cloneString(nextName);

                context->topDef = NULL;

                return false;
            }

            InterpreterScope* curScope = NULL;
            for(
                    curScope = stackPop(&context->Frame);
                    curScope == NULL? 0 : curScope->curDef != target;
                    curScope = stackPop(&context->Frame)
               )
            {
                destroyInterpreterScope(curScope);
            }

            //Cannot happen since we check if the target is the topDef before.
            assert(curScope != NULL);

            destroyInterpreterScope(curScope);

            resolveNextSpec((InterpreterScope*)stackPeek(&context->Frame), nextName);
        }
        else
        {
            fprintf(stderr,
                    "Unknown scope next type %s, aborting...",
                    symbolTypeToCString(scope->Next->Type)
                   );
            abort();

            context->topDef = NULL;
            return false;
        }

        return true;
    }

    //Aliases
    Symbol* toExec = scope->curSpec->Executable;
    SymbolTable values = scope->curSpec->executableValues;

    if(toExec->Type == STInstruction)
    {
        Instruction* inst = toExec->Data.STInstruction;

        List errors = {0};
        iterateListOfStringForward(inst->Values, curNode)
        {
            if(getSymbolTable(values, curNode->Name) == NULL)
            {
                appendPointerToListOfPointer(
                    createMissingSymbolErrorSpec(
                        curNode->Name, SAValueList, *scope->curSpec
                    ),
                    &errors
                );
            }
        }

        if(errors.Count > 0)
        {
            context->Result.Type = IESemantic;
            context->Result.Data.IESemantic.semanticsResult.Errors = errors;
            return false;
        }

        String jump = inst->Execute(values);
        resolveNextSpec(scope, jump);
    }
    else if(toExec->Type == STDef)
    {
        //Note: resolution function checks if we have all symbols
        SemanticAnalysisResult result = resolveSemanticDefSymbols(toExec->Data.STDef, values);

        if(result.Errors.Count > 0)
        {
            context->Result.Type = IESemantic;
            context->Result.Data.IESemantic.semanticsResult = result;

            return false;
        }

        InterpreterScope* newScope = createInterpreterScope(toExec->Data.STDef);

        stackPush(&context->Frame, newScope);
    }
    else
    {
        fprintf(stderr,
                "Unknown executable symbol type, %s, aborting... ",
                symbolTypeToCString(toExec->Type)
               );
        abort();

        return false;
    }

    return true;

}

void printInterpretationResult(InterpretationResult result, FILE* stream)
{

    switch(result.Type)
    {
        case IRunning:
            {
                fputs("Interpreter is still running \n", stream);
            } break;
        case ISuccessful:
            {
                fputs("Interpretation successful with result `", stream);
                writeString(result.Data.ISuccessful.Jump, stream);
                fputs("`.\n", stream);
            } break;
        case IESemantic:
            {
                fputs("Interpreter ran into semantic errors. Printing:\n", stream);
                printSemanticAnalysisResult(result.Data.IESemantic.semanticsResult, stream);
            } break;
    }

}

void finalizeInterpreter(InterpreterContext* context)
{

    for(void* scope = stackPop(&context->Frame); scope != NULL; scope = stackPop(&context->Frame))
    {
        free(scope);
    }

    finalizeStack(&context->Frame);

    switch(context->Result.Type)
    {
        case ISuccessful:
            {
                finalizeString(&context->Result.Data.ISuccessful.Jump);
            } break;
        case IESemantic:
            {
                finalizeSemanticAnalysisResult(
                    &context->Result.Data.IESemantic.semanticsResult
                );
            } break;
        default: break;
    }

    *context = (struct InterpreterContext){0};

}

void destroyInterpreterScope(InterpreterScope* scope)
{
    free(scope);
}

