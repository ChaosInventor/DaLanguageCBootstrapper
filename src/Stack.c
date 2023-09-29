#include <assert.h>
#include <stdlib.h>
#include "DaLanguageCBootstrap/Stack.h"

Stack initializeStack(size_t size)
{

    assert(size > 0);

    Stack ret = {0};

    ret.Buffer = calloc(size, sizeof(void*));
    assert(ret.Buffer != NULL);

    ret.Top = 0;
    ret.Size = size;

    return ret;

}

void* stackPush(Stack* stack, void* push)
{

    assert(stack != NULL);

    if(stack->Top == stack->Size)
    {
        void** newBuffer = realloc(stack->Buffer, sizeof(void*) * stack->Size * 2);
        assert(newBuffer != NULL);

        stack->Buffer = newBuffer;
        stack->Size *= 2;
    }

    void* ret = stack->Top == 0? NULL:stack->Buffer[stack->Top - 1];

    stack->Buffer[stack->Top++] = push;

    return ret;

}

void* stackPeek(Stack* stack)
{
    assert(stack != NULL);
    return stack->Top == 0? NULL:stack->Buffer[stack->Top - 1];
}

void* stackPop(Stack* stack)
{
    assert(stack != NULL);
    return stack->Top == 0? NULL:stack->Buffer[--stack->Top];
}

void finalizeStack(Stack* stack)
{

    assert(stack != NULL);

    free(stack->Buffer);

    *stack = (struct Stack){0};

}

