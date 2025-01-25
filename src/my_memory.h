// my_memory.h
#pragma once

#include "data_types.h"


#define Bytes(n) (n)
#define Kilobytes(n) (1024 * Bytes(n))
#define Megabytes(n) (1024 * Kilobytes(n))
#define Gigabytes(n) (1024 * Megabytes(n))


struct MAllocator;

typedef void* AllocateFunc(MAllocator* allocator, uint64 size);
typedef void DeallocateFunc(MAllocator* allocator, void* data);

struct MAllocator
{
    AllocateFunc* allocate;
    DeallocateFunc* deallocate;
};

inline void* AllocateMem(MAllocator* allocator, uint64 size)
{
    return allocator->allocate(allocator, size);
}


struct MemoryArena : MAllocator
{
    uint32 capacity;
    uint32 size; // in bytes
    void* ptr;
};


inline void DeallocateMem(MAllocator* allocator, void* data) {
    if (allocator->deallocate == NULL) { return; }
    allocator->deallocate(allocator, data);
}

void* PushSizeMemoryArena(MemoryArena* arena, uint64 size);
void AllocateMemoryArena(MemoryArena* arena, uint64 capacity);
