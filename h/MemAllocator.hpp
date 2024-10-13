#ifndef OS1_MEMORY_ALLOCATOR_
#define OS1_MEMORY_ALLOCATOR_

#include "../lib/hw.h"

class MemAllocator {
public:

    static MemAllocator* get();

    static void* mem_alloc();
    static int mem_free();

    static void* mem_alloc_argument(size_t size_in_bytes);
    static int mem_free_argument(void* ptr);

    static void initialize();

private:
    MemAllocator(const MemAllocator&) = default;
    MemAllocator& operator = (const MemAllocator&) = default;

    static MemAllocator myMemoryAllocator;
    MemAllocator() {}

    struct FreeMem
    {
        FreeMem* next;
        FreeMem* prev;
        size_t size;
    };


    static FreeMem* freeMemHead;
    static FreeMem* usedMemHead;


    static void tryToJoin(FreeMem* curr);

};


#endif