#include "DaLanguageCBootstrap/AST.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

AbstractDef* allocateAbstractDef()
{

    AbstractDef* ret = malloc(sizeof(AbstractDef));
    assert(ret != NULL);

    *ret = (struct AbstractDef){0};

    return ret;

}
AbstractSpec* allocateAbstractSpec()
{

    AbstractSpec* ret = malloc(sizeof(AbstractSpec));
    assert(ret != NULL);

    *ret = (struct AbstractSpec){0};

    return ret;

}


AbstractDef initializeAbstractDefFromPTNode(PTNode def)
{

    assert(def.Type == PDef);

    AbstractDef ret = {0};

    ret.Line = def.startLine;
    ret.Column = def.startColumn;

    PTNode* label = PTNodeParent(def);
    if(label == NULL? false: label->Type == PLabel)
        ret.Label = cloneString(label->Data);
    else
        ret.Label = initializeEmptyString();

    PTNode* header = findFirstPTChildWithType(def, PDefHeader);
    assert(header != NULL);
    assert(header->Type == PDefHeader);

    ret.Values = initializeListOfStringFromPTNode(findFirstPTChildWithType(*header, PUnlabeledValueList));
    ret.Jumps = initializeListOfStringFromPTNode(findLastPTChildWithType(*header, PUnlabeledJumpList));

    PTNode* body = findFirstPTChildWithType(def, PDefBody);
    assert(body != NULL);
    assert(body->Type == PDefBody);

    iteratePTNodeChildrenForward(*body, curNode)
    {
        if(curNode->Type == PLabel)
        {
            PTNode* child = findFirstPTChildWithType(*curNode, PLabelMarker);
            PTNodeAdvance(child);
            iteratePTNodeForward(child, node)
            {
                if(node->Type != PIgnorable)
                {
                    child = node;
                    break;
                }
            }
            assert(child != NULL);

            if(child->Type == PSpec)
            {
                AbstractSpec* spec = allocateAbstractSpec();

                *spec = initializeAbstractSpecFromPTNode(*child);

                appendPointerToListOfPointer(spec, &ret.Specs);
            }
            else
            {
                AbstractDef* def = allocateAbstractDef();

                *def = initializeAbstractDefFromPTNode(*child);

                appendPointerToListOfPointer(def, &ret.Defs);
            }
        }
        else if(curNode->Type == PSpec)
        {
            AbstractSpec* spec = allocateAbstractSpec();

            *spec = initializeAbstractSpecFromPTNode(*curNode);

            appendPointerToListOfPointer(spec, &ret.Specs);
        }
    }

    return ret;

}
AbstractSpec initializeAbstractSpecFromPTNode(PTNode spec)
{

    assert(spec.Type == PSpec);

    AbstractSpec ret = {0};

    ret.Line = spec.startLine;
    ret.Column = spec.startColumn;

    PTNode* label = PTNodeParent(spec);
    if(label == NULL? false: label->Type == PLabel)
        ret.Label = cloneString(label->Data);
    else
        ret.Label = initializeEmptyString();

    PTNode* name = findFirstPTChildWithType(spec, PName);
    assert(name != NULL);
    assert(name->Type = PName);

    ret.Referent = cloneString(name->Data);

    ret.Values = initializeLabeledListFromPTNode(findFirstPTChildWithType(spec, PLabeledValueList));
    ret.Jumps = initializeLabeledListFromPTNode(findFirstPTChildWithType(spec, PLabeledJumpList));

    return ret;

}

void initializePTNodeFromAbstractDefAt(PTNode* ret, AbstractDef def, PT* pt, PTNode* root)
{

    assert(pt != NULL);
    assert(root != NULL);

    *ret = (struct PTNode){0};;

    ret->Type = PDef;

    PTNode* node = allocatePTNode();

    if(!stringIsEqualToString(def.Label, EMPTY_STRING))
    {
        initializePTLabelNodeAt(node, def.Label, pt, root);
        pushPTNodeToFront(ret, node, pt);
        node = allocatePTNode();
    }
    else
    {
        pushPTNodeToFront(ret, root, pt);
    }

    PTNode* header = createPTNodeType(PDefHeader, pt, ret);

    initializePTNodeFromListOfStringAt(node, def.Values, PUnlabeledValueList, pt, header);
    node = allocatePTNode();

    initializePTNodeFromListOfStringAt(node, def.Jumps, PUnlabeledJumpList, pt, header);
    node = allocatePTNode();

    PTNode* body = createPTNodeType(PDefBody, pt, ret);

    initializePTNodeFromConstAt(node, "{", PDefStart, pt, body);
    node = allocatePTNode();

    iterateListOfPointerForward(def.Specs, curNode)
    {
        initializePTNodeFromAbstractSpecAt(node, *(AbstractSpec*)curNode->Pointer, pt, body);
        node = allocatePTNode();
    }

    iterateListOfPointerForward(def.Defs, curNode)
    {
        initializePTNodeFromAbstractDefAt(node, *(AbstractDef*)curNode->Pointer, pt, body);
        node = allocatePTNode();
    }

    initializePTNodeFromConstAt(node, "}", PDefStart, pt, body);

}
void initializePTNodeFromAbstractSpecAt(PTNode* ret, AbstractSpec spec, PT* pt, PTNode* root)
{

    assert(pt != NULL);
    assert(root != NULL);

    *ret = (struct PTNode){0};;

    ret->Type = PSpec;

    PTNode* node = allocatePTNode();

    if(!stringIsEqualToString(spec.Label, EMPTY_STRING))
    {
        initializePTLabelNodeAt(node, spec.Label, pt, root);
        pushPTNodeToFront(ret, node, pt);
        node = allocatePTNode();
    }
    else
    {
        pushPTNodeToFront(ret, root, pt);
    }

    initializePTNameNodeAt(node, spec.Referent, pt, ret);
    node = allocatePTNode();

    initializePTNodeFromListOfStringAt(node, spec.Values, PUnlabeledValueList, pt, ret);
    node = allocatePTNode();

    initializePTNodeFromListOfStringAt(node, spec.Jumps, PUnlabeledJumpList, pt, ret);
    node = allocatePTNode();

    initializePTNodeFromConstAt(node, ";", PSpecTerm, pt, ret);

}


void printAbstractSpec(AbstractSpec spec, FILE* stream)
{

    fprintf(
            stream,
            "Abstract spec created from %zi:%zi with referent \"",
            spec.Line, spec.Column
           );
    writeString(spec.Referent, stream);
    fprintf(stream, "\" and the label \"");
    writeString(spec.Label, stream);
    fprintf(stream, "\" has the following value list:\n");
    if(spec.Values.Count > 0) fprintf(stream, "\t");

    iterateLabeledListForward(spec.Values, curNode)
    {
        if(stringIsEqualToString(curNode->Label, EMPTY_STRING))
        {
            fprintf(stream, "<POSITIONAL>: ");
        }
        else
        {
            writeString(curNode->Label, stream);
            fprintf(stream, " ");
        }

        fprintf(stream, "<");
        writeString(curNode->Parameter, stream);
        if(curNode->Header.Next != NULL) fprintf(stream, ">, ");
        else fprintf(stream, ">\n");
    }

    if(spec.Jumps.Count > 0)
    {
        fprintf(stream, "... and the follow jump list:\n");
        fprintf(stream, "\t");
    }
    else
    {
        fprintf(stream, "... and it has no jump list.\n");
    }

    iterateLabeledListForward(spec.Jumps, curNode)
    {
        if(stringIsEqualToString(curNode->Label, EMPTY_STRING))
        {
            fprintf(stream, "<POSITIONAL>: ");
        }
        else
        {
            writeString(curNode->Label, stream);
            fprintf(stream, " ");
        }

        fprintf(stream, "<");
        writeString(curNode->Parameter, stream);
        if(curNode->Header.Next != NULL) fprintf(stream, ">, ");
        else fprintf(stream, ">\n");
    }

}
void printAbstractDef(AbstractDef def, FILE* stream)
{

    fprintf(
            stream, "Abstract def created from %zi:%zi with label \"",
            def.Line, def.Column
          );
    writeString(def.Label, stream);
    fprintf(stream, "\" has %zi values which are:\n", def.Values.Count);

    fprintf(stream, "\t(");
    iterateListOfStringForward(def.Values, value)
    {
        writeString(value->Name, stream);
        if(value->Header.Next != NULL) fprintf(stream, ", ");
    }
    fprintf(stream, ")\n");

    fprintf(stream, "... and the following jumps:\n");
    fprintf(stream, "\t[");
    iterateListOfStringForward(def.Jumps, value)
    {
        writeString(value->Name, stream);
        if (value->Header.Next != NULL) fprintf(stream, ", ");
    }
    fprintf(stream, "]\n");

    fprintf(stream, "... and the following specifications:\n");
    iterateListOfPointerForward(def.Specs, curNode)
    {
        AbstractSpec* spec = (AbstractSpec*)curNode->Pointer;
        printAbstractSpec(*spec, stream);
    }


    fprintf(stream, "... and the following definitions:\n");
    iterateListOfPointerForward(def.Defs, curNode)
    {
        AbstractDef* def = (AbstractDef*)curNode->Pointer;
        printAbstractDef(*def, stream);
    }

}


void finalizeAbstractDef(AbstractDef* def)
{

    finalizeString(&def->Label);
    finalizeListOfString(&def->Values);
    finalizeListOfString(&def->Jumps);

    iterateListOfPointerForward(def->Specs, curNode)
    {
        destroyAbstractSpec((AbstractSpec*)curNode->Pointer);
    }
    finalizeListOfPointer(&def->Specs);

    iterateListOfPointerForward(def->Defs, curNode)
    {
        destroyAbstractDef((AbstractDef*)curNode->Pointer);
    }
    finalizeListOfPointer(&def->Defs);

    *def = (struct AbstractDef){0};

}
void finalizeAbstractSpec(AbstractSpec* spec)
{

    finalizeString(&spec->Label);
    finalizeString(&spec->Referent);
    finalizeLabeledList(&spec->Values);
    finalizeLabeledList(&spec->Jumps);

    *spec = (struct AbstractSpec){0};

}

void destroyAbstractDef(AbstractDef* def)
{
    finalizeAbstractDef(def);
    free(def);
}
void destroyAbstractSpec(AbstractSpec* spec)
{
    finalizeAbstractSpec(spec);
    free(spec);
}

