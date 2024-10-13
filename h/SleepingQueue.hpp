#ifndef OS1_PROJEKAT_SLEEPING_QUEUE_HPP_
#define OS1_PROJEKAT_SLEEPING_QUEUE_HPP_

#include "../h/Queue.hpp"

class SleepingQueue {
public:
    static void insert(TCB* tcb);

    static void timerQueueUpdate();
private:
    static Queue sleepingQueue;
};
#endif