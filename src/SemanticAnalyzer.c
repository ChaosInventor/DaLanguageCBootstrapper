#include "DaLanguageCBootstrap/SemanticAnalyzer.h"
#include "DaLanguageCBootstrap/Instruction.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define addError(result, error) appendPointerToListOfPointer(error, &(result).Errors);

SemanticAnalysisError* allocateSemanticAnalysisError()
{

    SemanticAnalysisError* ret = malloc(sizeof(SemanticAnalysisError));

    *ret = (struct SemanticAnalysisError){0};

    return ret;

}
ParameterValue* allocateParameterValue()
{

    ParameterValue* ret = malloc(sizeof(ParameterValue));
    assert(ret != NULL);

    *ret = (struct ParameterValue){0};

    return ret;

}


void addSymbolToDefSymbols(Symbol* sym, String name, SemanticDef* def, SemanticAnalysisResult* result)
{

    assert(sym != NULL);
    assert(def != NULL);

    Symbol* oldSym = addSymbolTable(&def->Symbols, name, sym);
    if(oldSym != NULL)
    {
        addError(*result, createDuplicateErrorDef(*def, name, oldSym->Type, sym->Type));
        destroySymbol(oldSym);
    }

}
SemanticAnalysisResult initializeSemanticDefFromAbstractDefAt(
    SemanticDef* ret, AbstractDef def
)
{

    *ret = (struct SemanticDef){.Line = def.Line, .Column = def.Column};

    SemanticAnalysisResult result = {0};


    //FIXME: Overflows and overallocation possible
    size_t expectedSymbols =
        def.Values.Count + def.Jumps.Count + def.Defs.Count + def.Specs.Count;
    ret->Symbols = initializeSymbolTableSize(expectedSymbols * 2);


    iterateListOfStringForward(def.Values, value)
    {
        SymbolData data = {0};
        Symbol* sym = createSymbol(STUnknown, STUnresolved, data);

        addSymbolToDefSymbols(sym, value->Name, ret, &result);
    }
    ret->valuesEnd = (ListOfString*)ret->Symbols.symbolsList.Last;


    //FIXME: Properly deal with the jump's name
    if(def.Jumps.Count == 0)
    {
        ConstString(byteOne, "\1");

        SymbolData data = {.STJump = {.Def = ret}};
        data.STJump.Name = cloneString(byteOne);
        Symbol* onlyJump = createSymbol(STJump, STResolved, data);

        assert(addSymbolTable(&ret->Symbols, byteOne, onlyJump) == NULL);
    }
    else
    {
        iterateListOfStringForward(def.Jumps, jump)
        {
            SymbolData data = {.STJump = {.Def = ret}};
            data.STJump.Name = cloneString(jump->Name);
            Symbol* sym = createSymbol(STJump, STResolved, data);

            addSymbolToDefSymbols(sym, jump->Name, ret, &result);
        }
    }
    ret->jumpsEnd = (ListOfString*)ret->Symbols.symbolsList.Last;


    iterateListOfPointerForward(def.Defs, defNode)
    {
        AbstractDef* def = (AbstractDef*)defNode->Pointer;

        SemanticAnalysisResult otherResults = {0};
        SemanticDef* newDef = createSemanticDefFromAbstractDef(*def, &otherResults);
        if(newDef == NULL)
        {
            listMerge(&otherResults.Errors, &result.Errors);
            continue;
        }

        SymbolData data = {.STDef = newDef};
        Symbol* sym = createSymbol(STDef, STResolved, data);

        addSymbolToDefSymbols(sym, def->Label, ret, &result);
    }
    ret->defsEnd = (ListOfString*)ret->Symbols.symbolsList.Last;


    assert(def.Specs.Count > 0);

    //Generate empty symbols for all of them then resolve them in order to avoid
    //having to deal with nonlinear referencing.
    iterateListOfPointerForward(def.Specs, specNode)
    {
        AbstractSpec* spec = (AbstractSpec*)specNode->Pointer;

        if(!stringIsEqualToString(spec->Label, EMPTY_STRING))
        {
            SymbolData data = {.STSpec = NULL};
            Symbol* sym = createSymbol(STSpec, STUnresolved, data);

            addSymbolToDefSymbols(sym, spec->Label, ret, &result);
        }
    }
    ret->specsEnd = (ListOfString*)ret->Symbols.symbolsList.Last;

    iterateListOfPointerForward(def.Specs, specNode)
    {
        AbstractSpec* spec = (AbstractSpec*)specNode->Pointer;

        SemanticAnalysisResult otherResult = {0};
        SemanticSpec* newSpec = createSemanticSpecFromAbstractSpecAndContainingDef(*spec, ret, &otherResult);
        if(newSpec == NULL)
        {
            listMerge(&otherResult.Errors, &result.Errors);
            continue;
        }

        if(!stringIsEqualToString(spec->Label, EMPTY_STRING))
        {
            Symbol* sym = getSymbolTable(ret->Symbols, spec->Label);
            //Symbols should've been created in the loop before
            assert(sym != NULL);
            assert(sym->Type == STSpec);

            sym->Data.STSpec = newSpec;
        }

        appendPointerToListOfPointer(newSpec, &ret->Specs);
    }

    ret->executionStart = (SemanticSpec*)((ListOfPointer*)ret->Specs.First)->Pointer;

    if(result.Errors.Count > 0) finalizeSemanticDef(ret);

    return result;

}
SemanticAnalysisResult initializeSemanticSpecFromAbstractSpecAndContainingDefAt(
    SemanticSpec* ret, AbstractSpec spec, SemanticDef* containingDef
)
{

    //Assumes all previous names in abstract def have been symbolized.

    *ret = (struct SemanticSpec){.Line = spec.Line, .Column = spec.Column};
    SemanticAnalysisResult result = {0};

    //These get set during symbol resolution.
    ret->Executable = NULL;
    ret->executableValues = (struct SymbolTable){0};
    ret->executableJumps = (struct SymbolTable){0};

    ret->containingDef = containingDef;

    ret->Referent = getSymbolTable(containingDef->Symbols, spec.Referent);
    if(ret->Referent == NULL)
    {
        addError(result, createNoReferentError(spec.Referent, *ret));
    }

    ret->Values = initializeSymbolTableSize(spec.Values.Count);
    iterateLabeledListForward(spec.Values, value)
    {
        Symbol* sym = getSymbolTable(containingDef->Symbols, value->Parameter);
        if(sym == NULL)
        {
            addError(result, createMissingSymbolErrorSpec(value->Parameter, SAValueList, *ret));
            continue;
        }

        if(stringIsEqualToString(value->Label, EMPTY_STRING))
        {
            addSymbolTableNameless(&ret->Values, sym);
        }
        else
        {
            Symbol* old = addSymbolTable(&ret->Values, value->Label, sym);
            if(old != NULL)
            {
                appendValueToMultipleValuesError(&result, value->Label, SAValueList, EMPTY_STRING, old, *ret);
                appendValueToMultipleValuesError(&result, value->Label, SAValueList, value->Parameter, sym, *ret);
            }
        }
    }

    ret->Jumps = initializeSymbolTableSize(spec.Jumps.Count);
    iterateLabeledListForward(spec.Jumps, jump)
    {
        Symbol* sym = getSymbolTable(containingDef->Symbols, jump->Parameter);
        if(sym == NULL)
        {
            addError(result, createMissingSymbolErrorSpec(jump->Parameter, SAJumpList, *ret));
            continue;
        }
        switch(sym->Type)
        {
            case STJump:
            case STSpec:
                break;

            default:
                {
                    addError(result, createIncorrectSymbolTypeErrorSpec(sym->Type, *ret, 2, STJump, STSpec));
                    continue;
                } break;
        }

        if(stringIsEqualToString(jump->Label, EMPTY_STRING))
        {
            addSymbolTableNameless(&ret->Jumps, sym);
        }
        else
        {
            Symbol* old = addSymbolTable(&ret->Jumps, jump->Label, sym);
            if(old != NULL)
            {
                appendValueToMultipleValuesError(&result, jump->Label, SAValueList, EMPTY_STRING, old, *ret);
                appendValueToMultipleValuesError(&result, jump->Label, SAValueList, jump->Parameter, sym, *ret);
            }
        }
    }

    if(result.Errors.Count > 0) finalizeSemanticSpec(ret);

    return result;

}


SemanticDef* createSemanticDefFromAbstractDef(
    AbstractDef def, SemanticAnalysisResult* result
)
{

    SemanticDef* ret = malloc(sizeof(SemanticDef));
    assert(ret != NULL);

    *result = initializeSemanticDefFromAbstractDefAt(ret, def);
    if(result->Errors.Count > 0)
    {
        free(ret);
        return NULL;
    }
    else return ret;

}
SemanticSpec* createSemanticSpecFromAbstractSpecAndContainingDef(
    AbstractSpec spec, SemanticDef* containingDef,
    SemanticAnalysisResult* result
)
{

    SemanticSpec* ret = malloc(sizeof(SemanticSpec));
    assert(ret != NULL);

    *result = initializeSemanticSpecFromAbstractSpecAndContainingDefAt(ret, spec, containingDef);
    if(result->Errors.Count > 0)
    {
        free(ret);
        return NULL;
    }
    else return ret;

}

SemanticAnalysisError* createDuplicateErrorDef(
    SemanticDef def, String name, SymbolType old, SymbolType new
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEDuplicateSymbol;
    ret->Location = SADef;
    ret->Line = def.Line;
    ret->Column = def.Column;
    ret->Data.SAEDuplicateSymbol.duplicateName = cloneString(name);
    ret->Data.SAEDuplicateSymbol.oldType = old;
    ret->Data.SAEDuplicateSymbol.newType = new;

    return ret;

}
SemanticAnalysisError* createDuplicateErrorSpec(
    SemanticSpec spec, String name, SymbolType old, SymbolType new
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEDuplicateSymbol;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAEDuplicateSymbol.duplicateName = cloneString(name);
    ret->Data.SAEDuplicateSymbol.oldType = old;
    ret->Data.SAEDuplicateSymbol.newType = new;

    return ret;

}

SemanticAnalysisError* createNoReferentError(String name, SemanticSpec spec)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAENoReferent;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAENoReferent.Name = cloneString(name);

    return ret;

}

SemanticAnalysisError* createMissingSymbolErrorDef(
    String name, SemanticListType location, SemanticDef def
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEMissingSymbol;
    ret->Location = SADef;
    ret->Line = def.Line;
    ret->Column = def.Column;
    ret->Data.SAEMissingSymbol.Name = cloneString(name);
    ret->Data.SAEMissingSymbol.Location = location;

    return ret;

}
SemanticAnalysisError* createMissingSymbolErrorSpec(
    String name, SemanticListType location, SemanticSpec spec
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEMissingSymbol;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAEMissingSymbol.Name = cloneString(name);
    ret->Data.SAEMissingSymbol.Location = location;

    return ret;

}

SemanticAnalysisError* createMultipleParameterValuesError(
    String parameter, SemanticListType type, SemanticSpec spec
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEMultipleParameterValues;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAEMultipleParameterValues.Parameter = cloneString(parameter);
    ret->Data.SAEMultipleParameterValues.Values = (struct List){0};
    ret->Data.SAEMultipleParameterValues.Type = type;

    return ret;

}

SemanticAnalysisError* createReferentCycleError(
    SemanticSpec** specs, size_t count, SemanticDef def
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEReferentCycle;
    ret->Location = SADef;
    ret->Line = def.Line;
    ret->Column = def.Column;

    ret->Data.SAEReferentCycle.Specs = calloc(count, sizeof(SemanticSpec*));
    assert(ret->Data.SAEReferentCycle.Specs != NULL);

    ret->Data.SAEReferentCycle.Count = count;

    for(size_t i = 0; i < count; ++i)
    {
        ret->Data.SAEReferentCycle.Specs[i] = specs[i];
    }

    return ret;

}

SemanticAnalysisError* createIncorrectSymbolTypeErrorSpec_v(
    SymbolType given, SemanticSpec spec, size_t acceptableSize, va_list args
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEIncorrectSymbolType;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAEIncorrectSymbolType.Given = given;
    ret->Data.SAEIncorrectSymbolType.acceptableSize = acceptableSize;
    ret->Data.SAEIncorrectSymbolType.acceptableTypes = calloc(acceptableSize, sizeof(SymbolType));
    assert(ret->Data.SAEIncorrectSymbolType.acceptableTypes != NULL);

    for(size_t i = 0; i < acceptableSize; ++i)
    {
        ret->Data.SAEIncorrectSymbolType.acceptableTypes[i] = va_arg(args, SymbolType);
    }

    return ret;

}
SemanticAnalysisError* createIncorrectSymbolTypeErrorSpec(
    SymbolType given, SemanticSpec spec, size_t acceptableSize, ...
)
{

    va_list args;
    va_start(args, acceptableSize);

    SemanticAnalysisError* ret = createIncorrectSymbolTypeErrorSpec_v(given, spec, acceptableSize, args);

    va_end(args);

    return ret;

}
SemanticAnalysisError* createIncorrectSymbolTypeErrorDef_v(
    SymbolType given, SemanticDef def, size_t acceptableSize, va_list args
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEIncorrectSymbolType;
    ret->Location = SADef;
    ret->Line = def.Line;
    ret->Column = def.Column;
    ret->Data.SAEIncorrectSymbolType.Given = given;
    ret->Data.SAEIncorrectSymbolType.acceptableSize = acceptableSize;
    ret->Data.SAEIncorrectSymbolType.acceptableTypes = calloc(acceptableSize, sizeof(SymbolType));
    assert(ret->Data.SAEIncorrectSymbolType.acceptableTypes != NULL);

    for(size_t i = 0; i < acceptableSize; ++i)
    {
        ret->Data.SAEIncorrectSymbolType.acceptableTypes[i] = va_arg(args, SymbolType);
    }

    return ret;

}
SemanticAnalysisError* createIncorrectSymbolTypeErrorDef(
    SymbolType given, SemanticDef def, size_t acceptableSize, ...
)
{

    va_list args;
    va_start(args, acceptableSize);

    SemanticAnalysisError* ret = createIncorrectSymbolTypeErrorDef_v(given, def, acceptableSize, args);

    va_end(args);

    return ret;

}

SemanticAnalysisError* createUnknownParameterErrorDef(
    String name, SemanticListType location, SemanticDef def
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEUnknownParameter;
    ret->Location = SADef;
    ret->Line = def.Line;
    ret->Column = def.Column;
    ret->Data.SAEUnknownParameter.Name = cloneString(name);
    ret->Data.SAEUnknownParameter.Location = location;

    return ret;

}
SemanticAnalysisError* createUnknownParameterErrorSpec(
    String name, SemanticListType location, SemanticSpec spec
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAEUnknownParameter;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAEUnknownParameter.Name = cloneString(name);
    ret->Data.SAEUnknownParameter.Location = location;

    return ret;

}

SemanticAnalysisError* createTooManyParametersError(
    SemanticListType type, SemanticSpec spec
)
{

    SemanticAnalysisError* ret = allocateSemanticAnalysisError();

    ret->Type = SAETooManyParameters;
    ret->Location = SASpec;
    ret->Line = spec.Line;
    ret->Column = spec.Column;
    ret->Data.SAETooManyParameters.Location = type;

    return ret;

}


void appendStringToSemanticAnalysisError(
    String value, SemanticAnalysisError* error
)
{

    ParameterValue* pvalue = allocateParameterValue();
    pvalue->Value = cloneString(value);

    appendPointerToListOfPointer(pvalue, &error->Data.SAEMultipleParameterValues.Values);

}
void appendSymbolToSemanticAnalysisError(
    Symbol* symbol, SemanticAnalysisError* error
)
{

    ParameterValue* value = allocateParameterValue();
    value->symbolValue = symbol;

    appendPointerToListOfPointer(value, &error->Data.SAEMultipleParameterValues.Values);

}
void appendStringAndSymbolToSemanticAnalysisError(
    String value, Symbol* symbol, SemanticAnalysisError* error
)
{

    ParameterValue* pvalue = allocateParameterValue();
    pvalue->Value = cloneString(value);
    pvalue->symbolValue = symbol;

    appendPointerToListOfPointer(pvalue, &error->Data.SAEMultipleParameterValues.Values);

}

void appendValueToMultipleValuesError(
    SemanticAnalysisResult* results, String parameter, SemanticListType type,
    String symbolName, Symbol* symbol, SemanticSpec spec
)
{

    //Find if this error already exists, if so add it there, otherwise make a
    //new error
    iterateListOfPointerForward(results->Errors, curNode)
    {
        SemanticAnalysisError* error = (SemanticAnalysisError*)curNode->Pointer;

        if(error->Type == SAEMultipleParameterValues)
        {
            if(
                stringIsEqualToString(
                    error->Data.SAEMultipleParameterValues.Parameter,
                    parameter
                )
                &&
                error->Data.SAEMultipleParameterValues.Type == type
                &&
                error->Line == spec.Line
                &&
                error->Column == spec.Column
              )
            {
                appendStringAndSymbolToSemanticAnalysisError(symbolName, symbol, error);
                break;
            }
        }
    }

    SemanticAnalysisError* error = createMultipleParameterValuesError(parameter, type, spec);
    addError(*results, error);
    appendStringAndSymbolToSemanticAnalysisError(symbolName, symbol, error);

}


SemanticAnalysisResult resolveSemanticDefSymbols(
    SemanticDef* def, SymbolTable symbols
)
{

    assert(def != NULL);

    SemanticAnalysisResult ret = {0};

    iterateListOfStringNodeForward(value, (ListOfString*)def->Symbols.symbolsList.First)
    {
        Symbol* sym = getSymbolTable(symbols, value->Name);
        if(sym == NULL)
        {
            addError(ret, createMissingSymbolErrorDef(value->Name, SAValueList, *def));
            if(value == def->valuesEnd) break;
            else continue;
        }

        *getSymbolTable(def->Symbols, value->Name) = cloneSymbol(*sym);

        if(value == def->valuesEnd) break;
    }

    //Cannot do further resolution without having all values resolved.
    if(ret.Errors.Count > 0) return ret;

    Stack resolutionStack = initializeStack(def->Specs.Count);
    iterateListOfPointerForward(def->Specs, specNode)
    {
        SemanticSpec* semSpec = specNode->Pointer;

        stackPush(&resolutionStack, semSpec);

        bool cycle = false;
        while(semSpec->Referent->Type == STSpec)
        {
            //Check for referent loops
            for(size_t i = 0; i < resolutionStack.Top; ++i)
            {
                if(resolutionStack.Buffer[i] == semSpec->Referent->Data.STSpec)
                {
                    //TODO: Make sure the same cycle is not reported multiple times.
                    addError(ret, createReferentCycleError((SemanticSpec**)resolutionStack.Buffer + i, resolutionStack.Top - i, *def));
                    cycle = true;
                    break;
                }
            }

            stackPush(&resolutionStack, semSpec->Referent->Data.STSpec);
            semSpec = semSpec->Referent->Data.STSpec;
        }

        if(cycle)
        {
            resolutionStack.Top = 0;
            continue;
        }

        Symbol* executable = semSpec->Referent;
        List referentValues = {0};
        List referentJumps = {0};
        switch(semSpec->Referent->Type)
        {
            case STDef:
                {
                    SemanticDef* def = semSpec->Referent->Data.STDef;

                    iterateListOfStringForward(def->Symbols.symbolsList, curNode)
                    {
                        appendStringToListOfString(cloneString(curNode->Name), &referentValues);
                        if(curNode == def->valuesEnd) break;
                    }
                    iterateListOfStringNodeForward(curNode, (ListOfString*)def->valuesEnd->Header.Next)
                    {
                        appendStringToListOfString(cloneString(curNode->Name), &referentJumps);
                        if(curNode == def->jumpsEnd) break;
                    }

                } break;
            case STInstruction:
                {
                    Instruction* inst = semSpec->Referent->Data.STInstruction;

                    semSpec->Executable = semSpec->Referent;

                    referentValues = copyListOfString(inst->Values);
                    referentJumps = copyListOfString(inst->Jumps);
                } break;
            default:
                {
                    addError(ret, createIncorrectSymbolTypeErrorDef(semSpec->Referent->Type, *def, 2, STDef, STInstruction));
                    resolutionStack.Top = 0;
                    continue;
                } break;
        }

        for(
                SemanticSpec* curSpec = stackPop(&resolutionStack);
                curSpec != NULL;
                curSpec = stackPop(&resolutionStack)
           )
        {
            curSpec->Executable = executable;
            SemanticAnalysisResult other = resolveSemanticSpecSymbols(curSpec, &referentValues, &referentJumps);
            listMerge(&other.Errors, &ret.Errors);
        }

        finalizeListOfString(&referentValues);
        finalizeListOfString(&referentJumps);
    }

    finalizeStack(&resolutionStack);

    if(ret.Errors.Count > 0) finalizeSemanticDef(def);

    return ret;

}
static void NoOp(void* p) { (void)p; return; }
SemanticAnalysisResult resolveSemanticSpecSymbols(
    SemanticSpec* spec, List* executableValues, List* executableJumps
)
{

    assert(spec != NULL);

    finalizeSymbolTable(&spec->executableValues, NoOp);
    finalizeSymbolTable(&spec->executableJumps, NoOp);

    if(spec->Referent->Type == STSpec)
    {
        spec->executableValues = cloneSymbolTable(spec->Referent->Data.STSpec->executableValues);
        spec->executableJumps = cloneSymbolTable(spec->Referent->Data.STSpec->executableJumps);
    }
    else
    {
        spec->executableValues = initializeSymbolTableSize(executableValues->Count);
        spec->executableJumps = initializeSymbolTableSize(executableJumps->Count);
    }

    SemanticAnalysisResult ret = {0};

    ret = mergeSpecSymbolTables(spec->Values, &spec->executableValues, executableValues, *spec, SAValueList);
    SemanticAnalysisResult other = mergeSpecSymbolTables(spec->Jumps, &spec->executableJumps, executableJumps, *spec, SAJumpList);

    listMerge(&other.Errors, &ret.Errors);

    return ret;

}


SemanticAnalysisResult mergeSpecSymbolTables(
    SymbolTable from, SymbolTable* to, List* params, SemanticSpec spec,
    SemanticListType type
)
{

    assert(to != NULL);
    assert(params != NULL);

    SemanticAnalysisResult ret = {0};

    //Local copy, need in order to efficiently remove params from the original,
    //finalized at end of function.
    List paramsCopy = copyListOfString(*params);

    ListOfString* curParamNode = (ListOfString*)paramsCopy.First;
    ListOfPointer* curNameless = (ListOfPointer*)from.namelessValues.First;

    iterateListOfStringForward(from.symbolsList, curNode)
    {
        if(!stringIsEqualToString(curNode->Name, EMPTY_STRING))
        {
            curParamNode = findListOfStringWithName(paramsCopy, curNode->Name);
            if(!curParamNode)
            {
                addError(ret, createUnknownParameterErrorSpec(curNode->Name, type, spec));
                goto cancel;
            }

            Symbol* old = addSymbolTable(to, curParamNode->Name, getSymbolTable(from, curParamNode->Name));
            if(old != NULL)
            {
                appendValueToMultipleValuesError(
                    &ret, curParamNode->Name, type, EMPTY_STRING, old,
                    spec
                );
                appendValueToMultipleValuesError(
                    &ret, curParamNode->Name, type, curNode->Name,
                    getSymbolTable(from, curParamNode->Name), spec
                );
                goto cancel;
            }
        }
        else
        {
            if(curParamNode == NULL)
            {
                addError(ret, createTooManyParametersError(type, spec));
                goto cancel;
            }

            assert(curNameless != NULL);
            Symbol* old = addSymbolTable(to, curParamNode->Name, (Symbol*)curNameless->Pointer);
            if(old != NULL)
            {
                appendValueToMultipleValuesError(
                    &ret, curParamNode->Name, type, EMPTY_STRING, old,
                    spec
                );
                appendValueToMultipleValuesError(
                    &ret, curParamNode->Name, type, curNode->Name,
                    (Symbol*)curNameless->Pointer, spec
                );
                goto cancel;
            }

            curNameless = (ListOfPointer*)curNameless->Header.Next;
        }


        ListOfString* inOriginal = findListOfStringWithName(*params, curParamNode->Name);
        assert(inOriginal != NULL);

        listExtractNode(&inOriginal->Header, params);
        finalizeString(&inOriginal->Name);
        free(inOriginal);

        if(curParamNode != NULL) AdvanceListOfString(curParamNode);
    }

    cancel:
    finalizeListOfString(&paramsCopy);
    return ret;

}


void dumpSemanticDef(SemanticDef def, FILE* stream)
{

    fprintf(stream, "Semantic def extracted from %zd:%zd:\n", def.Line, def.Column);
    fputs("Symbol table:\n", stream);
    dumpSymbolTable(def.Symbols, stream);

    fputc('(', stream);
    iterateListOfStringForward(def.Symbols.symbolsList, curNode)
    {
        writeString(curNode->Name, stream);
        if(curNode != def.valuesEnd) fputs(", ", stream);
        else break;
    }
    fputs(")[", stream);
    iterateListOfStringNodeForward(curNode, (ListOfString*)def.valuesEnd->Header.Next)
    {
        writeString(curNode->Name, stream);
        if(curNode == def.jumpsEnd) break;
        else fputs(", ", stream);
    }
    fputs("]\n", stream);

    fprintf(stream, "Execution start is semantic spec at %p\n", (void*)def.executionStart);

    fputs("Named semantic specs in this def:\n", stream);
    iterateListOfStringNodeForward(curNode, (ListOfString*)def.defsEnd->Header.Next)
    {
        Symbol* sym = getSymbolTable(def.Symbols, curNode->Name);
        assert(sym != NULL);
        assert(sym->Type == STSpec);

        SemanticSpec* spec = sym->Data.STSpec;
        assert(spec != NULL);
        dumpSemanticSpec(*spec, stream);
        if(curNode == def.specsEnd) break;
    }
    fputs("Unnamed semantics specs in this def:\n", stream);
    iterateListOfPointerForward(def.Symbols.namelessValues, curNode)
    {
        Symbol* sym = (Symbol*)curNode->Pointer;
        assert(sym != NULL);

        if(sym->Type == STSpec)
        {
            SemanticSpec* spec = sym->Data.STSpec;
            dumpSemanticSpec(*spec, stream);
        }
    }

    fputs("Defs inside this def:\n", stream);
    if((ListOfString*)def.jumpsEnd->Header.Next == def.defsEnd)
    {
        fputs("None\n", stream);
    }
    else
    {
        iterateListOfStringNodeForward(curNode, (ListOfString*)def.jumpsEnd->Header.Next)
        {
            Symbol* sym = getSymbolTable(def.Symbols, curNode->Name);
            assert(sym != NULL);
            assert(sym->Type == STDef);

            SemanticDef* otherDef = sym->Data.STDef;
            assert(otherDef != NULL);
            dumpSemanticDef(*otherDef, stream);
            if(curNode == def.defsEnd) break;
        }
    }

}
void dumpSemanticSpec(SemanticSpec spec, FILE* stream)
{

    fprintf(
            stream,
            "Semantic spec extracted from %zd:%zd in semantic def at %p:\n",
            spec.Line, spec.Column, (void*)spec.containingDef
           );
    fprintf(
            stream, "Referent is a %s at %p\n", symbolTypeToCString(spec.Referent->Type),
            spec.Referent->Type == STDef? (void*)spec.Referent->Data.STDef
            : (void*)spec.Referent->Data.STSpec
           );
    fputs("Values symbol table is:\n", stream);
    dumpSymbolTable(spec.Values, stream);
    fputs("Jumps symbol table is:\n", stream);
    dumpSymbolTable(spec.Jumps, stream);

    if(spec.Executable != NULL)
    {
        fputs("Executable is a ", stream);
        switch(spec.Executable->Type)
        {
            case STDef: fprintf(stream, "semantic def at %p\n", (void*)spec.Executable->Data.STDef); break;
            case STInstruction: fprintf(stream, "instruction at %p\n", (void*)spec.Executable->Data.STInstruction); break;
            default: fputs("<ENUM ERROR>\n", stream); break;
        }

        fputs("Executable values symbol table is:\n", stream);
        dumpSymbolTable(spec.executableValues, stream);
        fputs("Executable jumps symbol table is:\n", stream);
        dumpSymbolTable(spec.executableJumps, stream);
    }
    else fputs("Executable is undefined.\n", stream);

}

void printSemanticAnalysisResult(SemanticAnalysisResult result, FILE* stream)
{

    iterateListOfPointerForward(result.Errors, curNode)
    {
        SemanticAnalysisError* error = (SemanticAnalysisError*)curNode->Pointer;

        fprintf(stream,
                "Error in %s at %zi:%zi -- ",
                semanticAnalysisErrorLocationToCString(error->Location),
                error->Line,error->Column
               );
        switch(error->Type)
        {
            case SAEDuplicateSymbol:
                {
                    fprintf(stream,
                            "duplicate symbol, old one was a %s new one is a "
                            "%s\n",
                            symbolTypeToCString(error->Data.SAEDuplicateSymbol.oldType),
                            symbolTypeToCString(error->Data.SAEDuplicateSymbol.newType)
                           );
                } break;
            case SAENoReferent:
                {
                    fprintf(stream, "no referent with name `");
                    writeString(error->Data.SAENoReferent.Name, stream);  
                    fprintf(stream, "`\n");
                } break;
            case SAEMissingSymbol:
                {
                    fprintf(stream, "missing symbol of name `");
                    writeString(error->Data.SAEMissingSymbol.Name, stream);
                    fprintf(stream,
                            "` in %s\n",
                            semanticListTypeToCString(error->Data.SAEMissingSymbol.Location)
                           );
                } break;
            case SAEMultipleParameterValues:
                {
                    fprintf(stream, "multiple values given to parameter `");
                    writeString(error->Data.SAEMultipleParameterValues.Parameter, stream);
                    fprintf(stream,
                            "` in %s, those being:\n ",
                            semanticListTypeToCString(
                                error->Data.SAEMultipleParameterValues.Type
                            )
                           );

                    iterateListOfPointerForward(error->Data.SAEMultipleParameterValues.Values, valueNode)
                    {
                        ParameterValue* value = (ParameterValue*)valueNode->Pointer;
                        fprintf(stream,
                                "\tType: %s, Name: `",
                                value->symbolValue == NULL? "Unknown" :
                                symbolTypeToCString(value->symbolValue->Type)
                               );
                        writeString(value->Value, stream);
                        fprintf(stream, "`\n");
                    }
                } break;
            case SAEReferentCycle:
                {
                    fprintf(stream, "referent cycle consisting of:\n");
                    for(size_t i = 0; i < error->Data.SAEReferentCycle.Count; ++i)
                    {
                        SemanticSpec* spec = error->Data.SAEReferentCycle.Specs[i];
                        fprintf(stream,
                                "\tSpecification at %zi:%zi\n",
                                spec->Line,
                                spec->Column
                               );
                    }
                } break;
            case SAEIncorrectSymbolType:
                {
                    fprintf(stream,
                            "incorrect symbol type, was given %s, but only: ",
                            symbolTypeToCString(error->Data.SAEIncorrectSymbolType.Given)
                           );
                    for(size_t i = 0; i < error->Data.SAEIncorrectSymbolType.acceptableSize; ++i)
                    {
                        fprintf(stream,
                                "%s",
                                symbolTypeToCString(
                                    error->Data.SAEIncorrectSymbolType.acceptableTypes[i]
                                )
                               );
                        if(i < error->Data.SAEIncorrectSymbolType.acceptableSize - 1)
                        {
                            fprintf(stream, ", ");
                        }
                    }
                    fprintf(stream, "; are acceptable\n");
                } break;
            case SAEUnknownParameter:
                {
                    fprintf(stream, "parameter with name `");
                    writeString(error->Data.SAEUnknownParameter.Name, stream);
                    fprintf(stream,
                            "` does not exist in %s\n",
                            semanticListTypeToCString(
                                error->Data.SAEUnknownParameter.Location
                            )
                           );
                } break;
            case SAETooManyParameters:
                {
                    fprintf(stream,
                            "too many parameters passed to %s\n",
                            semanticListTypeToCString(
                                error->Data.SAETooManyParameters.Location
                            )
                           );
                } break;
            default:
                {
                    fprintf(stderr, "Unknown semantic analysis error type, cannot print. Aborting...\n");
                    abort();
                } break;
        }
    }

}


const char* semanticAnalysisErrorLocationToCString(
    SemanticAnalysisErrorLocation location
)
{

    switch(location)
    {
        case SASpec: return "Spec";
        case SADef: return "Def";
        default: return "Unknown";
    }

}
const char* semanticListTypeToCString(SemanticListType type)
{

    switch(type)
    {
        case SAValueList: return "ValueList";
        case SAJumpList: return "JumpList";
        default: return "Unknown";
    }
}


void finalizeSemanticDef(SemanticDef* def)
{

    assert(def != NULL);

    if(def->Symbols.Table.BufferSize == 0) return;

    ListOfString* valuesStart = (ListOfString*)def->Symbols.symbolsList.First;
    ListOfString* jumpsStart = NULL;
    ListOfString* defsStart = NULL;
    ListOfString* specsStart = NULL;

    iterateListOfStringNodeForward(curNode, valuesStart)
    {
        free(getSymbolTable(def->Symbols, curNode->Name));
        if(curNode == def->valuesEnd)
        {
            jumpsStart = curNode;
            break;
        }
    }

    AdvanceListOfString(jumpsStart);
    iterateListOfStringNodeForward(curNode, jumpsStart)
    {
        destroySymbol(getSymbolTable(def->Symbols, curNode->Name));
        if(curNode == def->jumpsEnd)
        {
            defsStart = curNode;
            break;
        }
    }

    if(def->jumpsEnd != def->defsEnd)
    {
        AdvanceListOfString(defsStart);
        iterateListOfStringNodeForward(curNode, defsStart)
        {
            destroySymbol(getSymbolTable(def->Symbols, curNode->Name));
            if(curNode == def->defsEnd)
            {
                specsStart = curNode;
                break;
            }
        }
    }
    else specsStart = defsStart;

    AdvanceListOfString(specsStart);
    iterateListOfStringNodeForward(curNode, specsStart)
    {
        free(getSymbolTable(def->Symbols, curNode->Name));
        if(curNode == def->specsEnd) break;
    }

    iterateListOfPointerForward(def->Specs, curNode)
    {
        destroySemanticSpec((SemanticSpec*)curNode->Pointer);
    }
    finalizeSymbolTable(&def->Symbols, NoOp);
    finalizeListOfPointer(&def->Specs);

    *def = (struct SemanticDef){0};

}
void finalizeSemanticSpec(SemanticSpec* spec)
{

    assert(spec != NULL);

    finalizeSymbolTable(&spec->Values, NoOp);
    finalizeSymbolTable(&spec->Jumps, NoOp);
    finalizeSymbolTable(&spec->executableValues, NoOp);
    finalizeSymbolTable(&spec->executableJumps, NoOp);

    *spec = (struct SemanticSpec){0};

}
void finalizeSemanticAnalysisResult(SemanticAnalysisResult* result)
{

    assert(result != NULL);

    iterateListOfPointerForward(result->Errors, curNode)
    {
        destroySemanticAnalysisError((SemanticAnalysisError*)curNode->Pointer);
    }

    finalizeListOfPointer(&result->Errors);

    *result = (struct SemanticAnalysisResult){0};

}
void finalizeSemanticAnalysisError(SemanticAnalysisError* error)
{

    assert(error != NULL);

    switch(error->Type)
    {
        case SAEDuplicateSymbol:
            {
                finalizeString(&error->Data.SAEDuplicateSymbol.duplicateName);
            } break;
        case SAENoReferent:
            {
                finalizeString(&error->Data.SAENoReferent.Name);
            } break;
        case SAEMissingSymbol:
            {
                finalizeString(&error->Data.SAEMissingSymbol.Name);
            } break;
        case SAEMultipleParameterValues:
            {
                finalizeString(&error->Data.SAEMultipleParameterValues.Parameter);

                iterateListOfPointerForward(error->Data.SAEMultipleParameterValues.Values, curNode)
                {
                    ParameterValue* value = (ParameterValue*)curNode->Pointer;

                    finalizeString(&value->Value);
                }
            } break;
        case SAEReferentCycle:
            {
                free(error->Data.SAEReferentCycle.Specs);
            } break;
        case SAEIncorrectSymbolType:
            {
                free(error->Data.SAEIncorrectSymbolType.acceptableTypes);
            } break;
        case SAEUnknownParameter:
            {
                finalizeString(&error->Data.SAEUnknownParameter.Name);
            } break;
        case SAETooManyParameters:
            {
            } break;
        default:
            {
                fprintf(stderr, "Unknown semantic analysis error type, cannot finalize. Aborting...\n");
                abort();
            } break;
    }

    *error = (struct SemanticAnalysisError){0};

}


void destroySemanticDef(SemanticDef* def)
{
    finalizeSemanticDef(def);
    free(def);
}
void destroySemanticSpec(SemanticSpec* spec)
{
    finalizeSemanticSpec(spec);
    free(spec);
}
void destroySemanticAnalysisResult(SemanticAnalysisResult* result)
{
    finalizeSemanticAnalysisResult(result);
    free(result);
}
void destroySemanticAnalysisError(SemanticAnalysisError* error)
{
    finalizeSemanticAnalysisError(error);
    free(error);
}

