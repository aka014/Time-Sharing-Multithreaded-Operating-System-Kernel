#ifndef OS1_TCB_HPP
#define OS1_TCB_HPP

#include "../lib/hw.h"
#include "../h/scheduler.hpp"
#include "../h/MemAllocator.hpp"

// Thread Control Block
class TCB {
public:
    enum Privilege {USER, SUPERVISOR};

    using Body = void (*)(void*);

    TCB(Body body, void* arg, uint64* stack = (uint64*) MemAllocator::mem_alloc_argument(DEFAULT_STACK_SIZE),
        uint64 timeSlice = DEFAULT_TIME_SLICE, Privilege p = TCB::USER) : body(body), stack(stack),
        context({(uint64) &threadWrapper,(uint64) ((uint8*)stack + DEFAULT_STACK_SIZE)}), timeSlice(timeSlice),
        finished(false), blocked(false), arg(arg), privilege(p) {}

    ~TCB() { MemAllocator::mem_free_argument(stack); }

    bool isFinished() const { return finished; }
    void setFinished(bool value) { finished = value; }

    bool isBlocked() const { return blocked; }
    void setBlocked(bool value) { blocked = value; }

    //void setPrivilege(Privilege p) { this->privilege = p; }
    Privilege getPrivilege() const { return this->privilege; }

    void setClosedSemaphore(bool value) { closedSemaphore = value; }
    bool getClosedSemaphore() const { return closedSemaphore; }

    uint64 getTimeSlice() const { return timeSlice; }

    time_t getSleepTime() const { return sleepTime; }
    void setSleepTime(time_t timeout) { sleepTime = timeout; }

    time_t getWaitTime() const { return waitTime; }
    void setWaitTime(time_t timeout) { waitTime = timeout; }

    void setSemaphoreTimeout(bool value) { semaphoreTimeout = value; }
    bool getSemaphoreTimeout() const { return semaphoreTimeout; }

    void* operator new(size_t size);
    void* operator new[](size_t size);

    void operator delete(void *ptr);
    void operator delete[](void *ptr);

    static void exitThread();
    static int createThread();
    static void sleepThread();

    static TCB *running;

    static void dispatch();

private:
    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64* stack;
    Context context;
    uint64 timeSlice;


    bool finished;
    bool blocked;
    void* arg;
    Privilege privilege;
    bool closedSemaphore = false;
    bool semaphoreTimeout = 0;

    uint64 fp = 0; //ovde cu upisivati sscratch tj. sp u odnosu na koji mogu da relativno adresiram do sacuvanih registara

    time_t sleepTime = 0;
    time_t waitTime = -1;

    friend class Riscv;
    friend class SleepingQueue;

    static void threadWrapper(); //ovo je pocetna tacka rada za nit

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static uint64 timeSliceCounter;

};


#endif
