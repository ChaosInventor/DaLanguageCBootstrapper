#ifndef DALCBOOT_AST
#define DALCBOOT_AST

#include <stdio.h>
#include "DaLanguageCBootstrap/Tree.h"
#include "DaLanguageCBootstrap/String.h"
#include "DaLanguageCBootstrap/ListOfString.h"
#include "DaLanguageCBootstrap/ListOfPointer.h"
#include "DaLanguageCBootstrap/LabeledList.h"
#include "DaLanguageCBootstrap/PT.h"

typedef struct AbstractDef
{

    String Label; //Empty for top def, must be nonempty for all other defs.
    List Values; //ListOfString
    List Jumps; //ListOfString
    List Defs; //ListOfPointer, each pointer is to an AbstractDef
    List Specs; //ListOfPointer, each pointer is to an AbstractSpec
    size_t Line;
    size_t Column;

} AbstractDef;
typedef struct AbstractSpec
{

    String Label;
    String Referent; //Must be nonempty and unique within the containing def
    List Values; //LabeledList
    List Jumps; //LabeledList
    size_t Line;
    size_t Column;

} AbstractSpec;


AbstractDef* allocateAbstractDef(void);
AbstractSpec* allocateAbstractSpec(void);


AbstractDef initializeAbstractDefFromPTNode(PTNode def);
AbstractSpec initializeAbstractSpecFromPTNode(PTNode spec);

void initializePTNodeFromAbstractDefAt(PTNode* ret, AbstractDef def, PT* pt, PTNode* root);
void initializePTNodeFromAbstractSpecAt(PTNode* ret, AbstractSpec spec, PT* pt, PTNode* root);


void printAbstractSpec(AbstractSpec spec, FILE* stream);
void printAbstractDef(AbstractDef def, FILE* stream);


void finalizeAbstractDef(AbstractDef* def);
void finalizeAbstractSpec(AbstractSpec* spec);

void destroyAbstractDef(AbstractDef* def);
void destroyAbstractSpec(AbstractSpec* spec);

#endif //DALCBOOT_AST

