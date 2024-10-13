#ifndef OS1_SCHEDULER_HPP
#define OS1_SCHEDULER_HPP

class TCB;
class Queue;

class Scheduler
{
private:
    static Queue readyThreadQueue;

public:
    static TCB* get();

    static void put(TCB *tcb);

};

#endif
