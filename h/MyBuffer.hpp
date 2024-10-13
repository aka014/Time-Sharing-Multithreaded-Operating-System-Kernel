#ifndef OS1_PROJEKAT_MYBUFFER_HPP_
#define OS1_PROJEKAT_MYBUFFER_HPP_

#include "../h/scb.hpp"

class MyBuffer {
public:
    MyBuffer() : spaceAvailable(size), itemAvailable(0) {}

    void* operator new(size_t size) {
        return MemAllocator::mem_alloc_argument(size);
    }

    void operator delete(void* ptr) {
        MemAllocator::mem_free_argument(ptr);
    }

    bool isEmpty() const {
        return head == tail;
    }

    void put(char c) {
        spaceAvailable.wait();

        buffer[tail] = c;
        tail = (tail + 1) % size;

        itemAvailable.signal();
    }

    char get() {
        itemAvailable.wait();

        char c = buffer[head];
        head = (head + 1) % size;

        spaceAvailable.signal();
        return c;
    }

private:
    int head = 0;
    int tail = 0;
    static const int size = 1024;
    SCB spaceAvailable, itemAvailable;
    char buffer[size];
};

#endif