

#include "dynamic_array.h"





void DynamicArrayAllocateDynamic(DynamicArray_Untyped* array, uint32 elementSize)
{
    ArrayChunk* newDynamic = NULL;
    newDynamic = (ArrayChunk*)AllocateMem(array->allocator, sizeof(ArrayChunk) + (array->elementsPerChunk * elementSize));

    newDynamic->nextChunk = NULL;

    if (array->headChunk == NULL) {
        array->headChunk = newDynamic;
        array->tailChunk = newDynamic;
    }
    else {
        ArrayChunk* dynamic = array->tailChunk;
        dynamic->nextChunk = newDynamic;
        array->tailChunk = dynamic->nextChunk;
    }

    array->chunkCount++;
}

void DynamicArrayEnsureCapacity(DynamicArray_Untyped* array, uint32 elementSize, uint32 capacity)
{

    //ASSERT(array->allocator != NULL);
    //ASSERT(array->elementsPerChunk > 0);

    if (array->elementsPerChunk == 0) {
        array->elementsPerChunk = 8;
    }

    if (array->chunkCount * array->elementsPerChunk < capacity) {
        uint32 dynamicsToAdd = ((capacity / array->elementsPerChunk) - array->chunkCount) + 1;

        for (int i = 0; i < dynamicsToAdd; i++) {
            DynamicArrayAllocateDynamic(array, elementSize);
        }
    }
}