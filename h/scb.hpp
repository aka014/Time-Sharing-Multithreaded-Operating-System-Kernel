#ifndef OS1_PROJEKAT_SCB_HPP_
#define OS1_PROJEKAT_SCB_HPP_

#include "tcb.hpp"
#include "../h/Queue.hpp"

// Semaphore Control Block
class SCB {
public:

    SCB(unsigned init);

    ~SCB();

    enum SemaphoreConstants{
        SEMDEAD = -1UL,
        TIMEOUT = -2UL
    };

    int wait();
    int signal();
    int tryWait();
    uint64 timedWait(time_t timeout);

    int value() const { return val; }

    void* operator new(size_t size);
    void* operator new[](size_t size);

    void operator delete(void *ptr);
    void operator delete[](void *ptr);

    static int semOpen();
    static int semClose();
    static int semWait();
    static int semSignal();
    static int semTryWait();
    static void semTimedWait();

    static void semaphoreTimerUpdate();
protected:
    void block();
    TCB* deblock();

private:
    int val;
    Queue blockedQueue;

    static SCB* head;
    static SCB* tail;

    SCB* next = nullptr;
};

#endif
