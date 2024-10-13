#include "../h/scheduler.hpp"
#include "../h/Queue.hpp"


Queue Scheduler::readyThreadQueue;

TCB *Scheduler::get()
{
    return readyThreadQueue.remove();
}

void Scheduler::put(TCB *tcb)
{
    readyThreadQueue.insert(tcb);
}
