#include "../h/scb.hpp"
#include "../h/Riscv.hpp"

SCB* SCB::head = nullptr;
SCB* SCB::tail = nullptr;

SCB::SCB(unsigned init) {
    val = (int)init;
}

SCB::~SCB() {
    TCB* tcb = blockedQueue.remove();
    while (tcb) {
        tcb->setClosedSemaphore(true);
        //ovde fali da je odblokirana nit(tcb->setBlocked(false)), pa zato verovatno nije radilo, ali ja sam pokrio u sem_close ovu petlju
        Scheduler::put(tcb);
        tcb = blockedQueue.remove();
    }
}

int SCB::wait() {
    if (--val < 0) block();
    if (TCB::running->getClosedSemaphore()) return -4; // semafor je zatvoren
    return 0;
}

int SCB::signal() {
    if (val++ < 0) deblock(); //ako ima niti koje cekaju pustaj
    return 0;
}

// da se vidi moze li da se uradi wait i da li ce posle val 0 na sem
int SCB::tryWait() {
    int test = val - 1;
    if (test > 0) {
        wait();
        return 1;
    }
    else if (test == 0) {
        wait();
        return 0;
    }
    return -1;
}

void* SCB::operator new(size_t size) {
    return MemAllocator::mem_alloc_argument(size);
}
void* SCB::operator new[](size_t size) {
    return MemAllocator::mem_alloc_argument(size);
}

void SCB::operator delete(void *ptr) {
    MemAllocator::mem_free_argument(ptr);
}
void SCB::operator delete[](void *ptr) {
    MemAllocator::mem_free_argument(ptr);
}

void SCB::block() {
    TCB::running->setBlocked(true);
    blockedQueue.insert(TCB::running);
    TCB::dispatch();
}

TCB* SCB::deblock() {
    TCB* tcb = blockedQueue.remove();
    if (tcb) {
        tcb->setBlocked(false);
        tcb->setClosedSemaphore(false);
        Scheduler::put(tcb);
    }
    return tcb;
}



int SCB::semOpen() {
    SCB** handle = (SCB**) Riscv::r_stack_a1();
    uint64 init = Riscv::r_stack_a2();
    int retval;

    *handle = new SCB(init);
    if (*handle != nullptr) {
        if(!head) head = (*handle);
        else tail->next = (*handle);
        tail = (*handle);

        retval = 0;
        Riscv::w_stack_a0(retval);
        return retval;
    }
    retval = -2; // neuspela alokacija
    Riscv::w_stack_a0(retval);
    return retval;
}

int SCB::semClose() {
    SCB* handle = (SCB*) Riscv::r_stack_a1();
    int retval;

    if (handle != nullptr) {
        SCB* curr = head, *prev = nullptr;
        while(curr && curr != handle){
            prev = curr;
            curr = curr->next;
        }
        if(curr){
            if(!prev) head = curr->next;
            else prev->next = curr->next;
            if(!curr->next) tail = prev;
        }

        TCB* cur = handle->deblock();
        while (cur) {
            cur = handle->deblock();
        }

        delete handle;
        handle = nullptr; //nece se vise otvarati isti sem??
        return 0;
    }

    retval = -1;
    Riscv::w_stack_a0(retval);
    return retval;
}

int SCB::semWait() {
    SCB* id = (SCB*) Riscv::r_stack_a1();
    int retval;

    if (id != nullptr) {
        retval = id->wait();
        Riscv::w_stack_a0(retval);
        return retval;
    }

    retval = -3; //prosledjen nullptr kao id
    Riscv::w_stack_a0(retval);
    return retval;
}

int SCB::semSignal() {
    SCB* id = (SCB*) Riscv::r_stack_a1();
    int retval;

    if (id != nullptr) {
        retval = id->signal();
        Riscv::w_stack_a0(retval);
        return retval;
    }

    retval = -3;
    Riscv::w_stack_a0(retval);
    return retval;
}

int SCB::semTryWait() {
    SCB* id = (SCB*) Riscv::r_stack_a1();
    int retval;

    if (id != nullptr) {
        retval = id->tryWait();
        Riscv::w_stack_a0(retval);
        return retval;
    }

    retval = -3;
    Riscv::w_stack_a0(retval);
    return retval;
}

void SCB::semaphoreTimerUpdate() {
    SCB* curr = head;
    while (curr) {
        int deblocked = curr->blockedQueue.semQueueTimerTick();
        curr->val += deblocked;
        curr = curr->next;
    }
    //treba proci svaki postojeci semafor tako da se moraju updateovati sem open i semclose
    //blocked queue. time update
    // povecati val semafora za povratnu vrednost fje iznad
}

uint64 SCB::timedWait(time_t timeout) {
    TCB::running->setWaitTime(timeout);
    if (--val < 0) block();
    TCB::running->setWaitTime(-1);
    if(TCB::running->getClosedSemaphore()){
        TCB::running->setClosedSemaphore(false);
        return SEMDEAD;
    }
    if(TCB::running->getSemaphoreTimeout()){
        TCB::running->setSemaphoreTimeout(false);
        return TIMEOUT;
    }
    return 0;
}

void SCB::semTimedWait() {
    SCB* id = (SCB*) Riscv::r_stack_a1();
    time_t timeout = (time_t) Riscv::r_stack_a2();
    int retval;

    if (id != nullptr) {
        retval = id->timedWait(timeout);
        Riscv::w_stack_a0(retval);
        return;
    }

    retval = -3;
    Riscv::w_stack_a0(retval);
}