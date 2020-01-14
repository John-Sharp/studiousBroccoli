#include "testMallocTracker.h"

typedef struct ptrTableNode
{
    void * ptr;
    size_t allocated;
} ptrTableNode;

#include "listHeaders/ptrTableList.h"
#include "listCode/ptrTableList.inc"

typedef struct allocatorTracker
{
    int bytesAllocated;
    int bytesFreed;
    ptrTableList * ptrTableList;
} allocatorTrackerStruct;
allocatorTrackerStruct allocatorTracker;
void allocatorTrackerReset()
{
    allocatorTracker.bytesAllocated = 0;
    allocatorTracker.bytesFreed = 0;

    // TODO free ptrTableList
}

jint allocatorTrackerGetBytesAllocated()
{
    return allocatorTracker.bytesAllocated;
}

jint allocatorTrackerGetBytesFreed()
{
    return allocatorTracker.bytesFreed;
}

bool ptrTableNodePtrCmp(const ptrTableNode * a, const ptrTableNode * b)
{
    return (a->ptr == b->ptr);
}

void * myMalloc(size_t x)
{
    if (mallocShouldFail && mallocShouldFail())
    {
        return NULL;
    }

    void * p = malloc(x);

    allocatorTracker.bytesAllocated += x;
    ptrTableNode * node = malloc(sizeof(node));
    node->ptr = p;
    node->allocated = x;

    allocatorTracker.ptrTableList = ptrTableListAdd(
        allocatorTracker.ptrTableList,
        node);

    return p;
}

void myFree(void * ptr)
{
    ptrTableNode node = {ptr, 0};
    ptrTableNode * foundNode;
    allocatorTracker.ptrTableList =  ptrTableListRm(
            allocatorTracker.ptrTableList,
            &node,
            ptrTableNodePtrCmp,
            &foundNode);

    allocatorTracker.bytesFreed += foundNode->allocated;
    free(foundNode);

    free(ptr);
}


