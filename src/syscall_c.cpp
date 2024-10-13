#include "../h/syscall_c.hpp"
#include "../h/MemAllocator.hpp"

// 0x01
// potrebno je poslati u blokovima a ne u bajtovima, to je zahtev projekta
void* mem_alloc(size_t size_in_bytes) {
    uint64 size_in_blocks = (size_in_bytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    __asm__ volatile ("mv a1, %0" : : "r" (size_in_blocks));
    __asm__ volatile ("mv a0, %0" : : "r" (0x01));

    __asm__ volatile ("ecall");

    uint64 address;
    __asm__ volatile("mv %0, a0" : "=r" (address));
    return (void*)address;
}

// 0x02
int mem_free(void* ptr) {
    __asm__ volatile ("mv a1, %0" : : "r" (ptr));
    __asm__ volatile ("mv a0, %0" : : "r" (0x02));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x11
int thread_create(thread_t* handle, void(*start_routine) (void*), void* arg) {

    uint64* stack = (uint64*)MemAllocator::mem_alloc_argument(DEFAULT_STACK_SIZE);

    __asm__ volatile ("mv a4, %0" : : "r" ((uint64)stack));
    __asm__ volatile ("mv a3, %0" : : "r" ((uint64)arg));
    __asm__ volatile ("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("mv a0, %0" : : "r" (0x11));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}


// 0x12
int thread_exit() {
    __asm__ volatile ("mv a0, %0" : : "r" (0x12));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x13
void thread_dispatch() {
    __asm__ volatile ("mv a0, %0" : : "r" (0x13));

    __asm__ volatile ("ecall");

}

// 0x21
int sem_open(sem_t* handle, unsigned init) {
    __asm__ volatile ("mv a2, %0" : : "r" (init));
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("mv a0, %0" : : "r" (0x21));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x22
int sem_close(sem_t handle) {
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("mv a0, %0" : : "r" (0x22));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x23
int sem_wait(sem_t id) {
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (0x23));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x24
int sem_signal(sem_t id) {
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (0x24));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x25
int sem_timedwait(sem_t id, time_t timeout) {
    __asm__ volatile ("mv a2, %0" : : "r" (timeout));
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (0x25));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x26
int sem_trywait (sem_t id) {
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a0, %0" : : "r" (0x26));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x31
int time_sleep(time_t t) {
    __asm__ volatile ("mv a1, %0" : : "r" (t));
    __asm__ volatile ("mv a0, %0" : : "r" (0x31));

    __asm__ volatile ("ecall");

    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

// 0x41
char getc() {
    __asm__ volatile ("mv a0, %0" : : "r" (0x41));

    __asm__ volatile ("ecall");

    uint64 c;
    __asm__ volatile ("mv %0, a0" : "=r"(c));
    return (char)c;
}

// 0x42
void putc(char c) {
    __asm__ volatile ("mv a1, %0" : : "r" (c));
    __asm__ volatile ("mv a0, %0" : : "r" (0x42));

    __asm__ volatile ("ecall");

}





