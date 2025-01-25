#pragma once

#include "my_memory.h"


struct ArrayChunk
{
    ArrayChunk* nextChunk;
    uint8 data[0];
};

struct DynamicArray_Untyped
{
    MAllocator* allocator;

    uint32 count;
    uint32 elementsPerChunk;

    uint32 chunkCount;

    ArrayChunk* headChunk;
    ArrayChunk* tailChunk;
};

template <typename T>
struct DynamicArray
{
    MAllocator* allocator;

    uint32 elementsPerChunk;

    uint32 count;
    uint32 chunkCount;

    ArrayChunk* headChunk;
    ArrayChunk* tailChunk;
    inline T& operator[](const int index) const
    {
        // (index / elementsPerChunk) to get the chunk
        // then index % chunkCount to get the index

        s32 chunkIndex = index / elementsPerChunk;
        ArrayChunk* chunk = headChunk;
        for (int32 i = 0; i < chunkIndex; i++)
        {
            chunk = chunk->nextChunk;
        }

        return *(T*)(chunk->data + (sizeof(T) * (index % elementsPerChunk)));
    }
};

template <typename T>
void DynamicArrayAllocateChunk(DynamicArray<T>* array)
{
    ArrayChunk* newChunk = (ArrayChunk*)AllocateMem(array->allocator, sizeof(ArrayChunk) + (array->elementsPerChunk * sizeof(T)));
    newChunk->nextChunk = NULL;

    if (array->headChunk == NULL)
    {
        array->headChunk = newChunk;
        array->tailChunk = newChunk;
    }
    else
    {
        ArrayChunk* chunk = array->tailChunk;
        chunk->nextChunk = newChunk;
        array->tailChunk = chunk->nextChunk;
    }

    array->chunkCount++;
}

////inline void* DynamicArrayGetData(DynamicArray_Untyped const* array, int32 elementSize, int32 index) {
////
////    s32 dynamicIndex = index / array->elementsPerChunk;
////    ArrayChunk* chunk = array->headChunk;
////    for (int32 i = 0; i < dynamicIndex; i++) {
////        chunk = chunk->nextChunk;
////    }
////
////    return ((uint8*)chunk->data + (elementSize * (index % array->elementsPerChunk)));
////}

template <typename T>
void DynamicArrayEnsureCapacity(DynamicArray<T>* array, uint32 capacity)
{
   // ASSERT(array->allocator != NULL);
    //ASSERT(array->elementsPerChunk > 0);

    if (array->chunkCount * array->elementsPerChunk < capacity)
    {
        uint32 chunksToAdd = ((capacity / array->elementsPerChunk) - array->chunkCount) + 1;

        for (int i = 0; i < chunksToAdd; i++)
        {
            DynamicArrayAllocateChunk(array);
        }
    }
}

void DynamicArrayAllocateDynamic(DynamicArray_Untyped* array, uint32 elementSize);
void DynamicArrayEnsureCapacity(DynamicArray_Untyped* array, uint32 elementSize, uint32 capacity);

template <typename T>
inline DynamicArray<T> MakeDynamicArray(MAllocator* allocator, uint32 elementsPerChunk, uint32 chunkCount = 1)
{
    DynamicArray<T> array = {};
    array.allocator = allocator;
    array.elementsPerChunk = elementsPerChunk;

    DynamicArrayEnsureCapacity(&array, chunkCount * elementsPerChunk);

    return array;
};

////template <typename T>
////inline void DeallocateDynamicArray(DynamicArray<T>* array) {
////    ArrayChunk* chunk = array->headChunk;
////    ArrayChunk* nextChunk = chunk->nextChunk;
////    while (chunk != NULL) {
////        DeallocateMem(array->allocator, chunk);
////
////        chunk = nextChunk;
////
////        if (chunk) {
////            nextChunk = nextChunk->nextChunk;
////        }
////    }
////}

template <typename T>
inline uint32 PushBack(DynamicArray<T>* array, T elem)
{
    DynamicArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->count++;
    (*array)[index] = elem;
    return index;
}

////template <typename T>
////inline T* PushBackPtr(DynamicArray<T>* array) {
////    DynamicArrayEnsureCapacity(array, array->count + 1);
////    uint32 index = array->count;
////    array->count++;
////    T* result = &(*array)[index];
////    memset(result, 0, sizeof(T));
////    return result;
////}
////
////template <typename T>
////inline bool PopBack(DynamicArray<T>* array, T* element = NULL)
////{
////    bool result = false;
////
////    if (array->count > 0) {
////        if (element != NULL) {
////            int32 index = array->count - 1;
////            *element = (*array)[index];
////        }
////        array->count--;
////        result = true;
////    }
////
////    return result;
////}

template <typename T>
inline void DynamicArrayClear(DynamicArray<T>* array)
{
    array->count = 0;
}


////inline void* PushBackPtr(DynamicArray_Untyped* array, u32 size)
////{
////    DynamicArrayEnsureCapacity(array, size, array->count + 1);
////    uint32 index = array->count;
////    array->count++;
////
////    void* ptr = DynamicArrayGetData(array, size, index);
////    memset(ptr, 0, size);
////    return ptr;
////}
////
////inline DynamicArray_Untyped MakeDynamicArray(MAllocator* allocator, uint32 size, uint32 elementsPerChunk, uint32 chunkCount = 1) {
////    DynamicArray_Untyped array = {};
////    array.allocator = allocator;
////
////    if (elementsPerChunk == 0) {
////        elementsPerChunk = 1;
////    }
////
////    array.elementsPerChunk = elementsPerChunk;
////
////    DynamicArrayEnsureCapacity(&array, size, elementsPerChunk);
////    return array;
////}