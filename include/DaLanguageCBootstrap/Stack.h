#ifndef DALCBOOT_STACK
#define DALCBOOT_STACK

#include <stddef.h>

typedef struct Stack
{
    void** Buffer;
    size_t Top;
    size_t Size;
} Stack;

Stack initializeStack(size_t size);

void* stackPush(Stack* stack, void* push);

void* stackPeek(Stack* stack);

void* stackPop(Stack* stack);

void finalizeStack(Stack* stack);

#endif //DALCBOOT_STACK
