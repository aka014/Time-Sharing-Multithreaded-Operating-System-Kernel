#include "../h/syscall_cpp.hpp"


void* operator new(size_t s) {
    return mem_alloc(s);
}

void operator delete(void* ptr) {
    mem_free(ptr);
}

// za niti koje se ne extenduju iz thread vec im se dodeljuje koju f-ju izvrsavaju
Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::~Thread() {
    delete myHandle;
    myHandle = nullptr;
}

int Thread::start() {
    if (myHandle == nullptr) return thread_create(&myHandle, body, arg);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

// telo f-je koju ce da izvrsavaju sve niti koje su izvedene iz thread, tj navesce ih da izvrse run
void Thread::wrapper(void* arg) {
    //ova fja ce biti body za niti pravljene pomocu Thread()
    if (arg) {
        Thread *thread = (Thread*) arg;
        thread->run();
    }
}

Thread::Thread() : myHandle(nullptr), body(Thread::wrapper), arg(this) {}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Semaphore::Semaphore(unsigned init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time) {
    return sem_timedwait(myHandle, time);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) : Thread(PeriodicThread::periodicThreadWrapper, this) {
    this->period = period;
}

void PeriodicThread::periodicThreadWrapper(void* arg) {
    PeriodicThread* thread = (PeriodicThread*) arg;
    while (thread->period != -1UL) {
        thread->periodicActivation();
        Thread::sleep(thread->period);
    }
}

void PeriodicThread::terminate() {
    this->period = -1UL;
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
