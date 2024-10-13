#include "../h/tcb.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/SleepingQueue.hpp"

TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;


void* TCB::operator new(size_t size) {
    return MemAllocator::mem_alloc_argument(size);
}
void* TCB::operator new[](size_t size) {
    return MemAllocator::mem_alloc_argument(size);
}

void TCB::operator delete(void *ptr) {
    MemAllocator::mem_free_argument(ptr);
}
void TCB::operator delete[](void *ptr) {
    MemAllocator::mem_free_argument(ptr);
}

int TCB::createThread() {
    TCB** handle = (TCB**) Riscv::r_stack_a1();
    TCB::Body body = (TCB::Body) Riscv::r_stack_a2();
    void* arg = (void*) Riscv::r_stack_a3();
    uint64* stack = (uint64*) Riscv::r_stack_a4();
    int retval;

    //arg moze biti nullptr ako je fja bez argumenata

    *handle = new TCB(body, arg, stack);
    if (*handle != nullptr) {
        Scheduler::put(*handle);
        retval = 0;
        Riscv::w_stack_a0(retval);
        return 0;
    }
    //ako alokacija ne uspe
    retval = -2;
    Riscv::w_stack_a0(retval);
    return -2;
}

void TCB::exitThread() {
    TCB::running->setFinished(true);
    Riscv::w_stack_a0(0);
    TCB::dispatch();
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    thread_exit();
}

void TCB::dispatch() {
    TCB* old = TCB::running;
    timeSliceCounter = 0;
    if (!old->isFinished() && !old->isBlocked()) Scheduler::put(old);
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::sleepThread() {
    time_t timeout = (time_t) Riscv::r_stack_a1();

    if (timeout <= 0) {
        Riscv::w_stack_a0(-1);
        return;
    }

    TCB::running->setSleepTime(timeout);
    SleepingQueue::insert(TCB::running);
    TCB::dispatch();

    Riscv::w_stack_a0(0);
}


