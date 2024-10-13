#include "../h/SleepingQueue.hpp"
#include "../h/scheduler.hpp"

Queue SleepingQueue::sleepingQueue;

void SleepingQueue::insert(TCB *tcb) {
    tcb->setBlocked(true);
    sleepingQueue.insertSleep(tcb);
}

void SleepingQueue::timerQueueUpdate() {
    while (sleepingQueue.peekFirst() && sleepingQueue.peekFirst()->sleepTime-- <= 0) {
        TCB* tcb = sleepingQueue.removeSleep();

        tcb->setSleepTime(0);
        tcb->setBlocked(false);

        Scheduler::put(tcb);
    }
}