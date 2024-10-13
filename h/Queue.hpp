#ifndef OS1_PROJEKAT_QUEUE_HPP_
#define OS1_PROJEKAT_QUEUE_HPP_

#include "../h/MemAllocator.hpp"
#include "../h/tcb.hpp"

class TCB; //ovo mozda moze biti problematicno

class Queue {
public:
    Queue() : head(nullptr), tail(nullptr) {}

    Queue(const Queue& q) = delete;
    Queue& operator= (const Queue& q) = delete;

    void insert(TCB* data);

    void insertSleep(TCB* data);

    TCB* remove();

    TCB* removeSleep();

    void removeTCB(TCB* tcb);

    TCB* peekFirst();

    int semQueueTimerTick(); //vraca broj odblokiranih niti zbog timeouta

private:

    struct Elem
    {
        TCB* data;
        Elem *next;

        Elem(TCB *data, Elem *next) : data(data), next(next) {}

        void* operator new(size_t size) {
            return MemAllocator::mem_alloc_argument(size);
        }

        void operator delete(void* ptr) {
            MemAllocator::mem_free_argument(ptr);
        }
    };

    void* operator new(size_t size) {
        return MemAllocator::mem_alloc_argument(size);
    }

    void* operator new[](size_t size) {
        return MemAllocator::mem_alloc_argument(size);
    }

    void operator delete(void* ptr) {
        MemAllocator::mem_free_argument(ptr);
    }

    void operator delete[](void* ptr) {
        MemAllocator::mem_free_argument(ptr);
    }



    Elem *head, *tail;
};

#endif