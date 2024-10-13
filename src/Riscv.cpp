#include "../h/Riscv.hpp"
#include "../h/scb.hpp"
#include "../lib/console.h"
#include "../h/MyConsole.hpp"
#include "../h/SleepingQueue.hpp"

#include "../test/printing.hpp" //koristim njihov print da lakse pratim sta se desava

uint64 Riscv::timeSliceCounter = 0;

void Riscv::popSppSpie() {

    if(TCB::running->getPrivilege() == TCB::SUPERVISOR) ms_sstatus(SSTATUS_SPP);
    else mc_sstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSyscall() {

    uint64 volatile sepc = r_sepc() + 4;
    uint64 volatile sstatus = r_sstatus();
    uint64 volatile operation_code = r_stack_a0();

    switch(operation_code) {
        case 0x01: {
            MemAllocator::mem_alloc();
            break;
        }

        case 0x02: {
            MemAllocator::mem_free();
            break;
        }

        case 0x11: {
            TCB::createThread();
            break;
        }

        case 0x12: {
            TCB::exitThread();
            break;
        }

        case 0x13: {
            TCB::dispatch();
            break;
        }

        case 0x21: {
            SCB::semOpen();
            break;
        }

        case 0x22: {
            SCB::semClose();
            break;
        }

        case 0x23: {
            SCB::semWait();
            break;
        }

        case 0x24: {
            SCB::semSignal();
            break;
        }

        case 0x25: {
            SCB::semTimedWait();
            break;
        }

        case 0x26: {
            SCB::semTryWait();
            break;
        }

        case 0x31: {
            TCB::sleepThread();
            break;
        }

        case 0x41: {
            MyConsole::consoleGetc();
            break;
        }

        case 0x42: {
            MyConsole::consolePutc();
            break;
        }

    }

    w_sstatus(sstatus);
    w_sepc(sepc);
}

void Riscv::handleConsoleInterrupt() {
    int irq = plic_claim();
    if (irq == 10) MyConsole::get(); //ako je prekid od konzole
    plic_complete(irq);
}

void Riscv::handleSupervisorTrap() {
    uint64 fp = TCB::running->fp;
    TCB::running->fp = r_sscratch();

    uint64 scause = r_scause();
    if (scause == USER_ECALL || scause == SYSTEM_ECALL) handleSyscall();
    else if (scause == TIMER_INT) handleTimerInterrupt();
    else if (scause == EXTERNAL_HARDWARE_INT) handleConsoleInterrupt();
    else {
        printInt(r_scause());
        stopEmulator(); //signali greske
    }

    TCB::running->fp = fp;
}

void Riscv::stopEmulator() {
    __asm__ volatile ("mv t6, %0" : : "r" (0x100000)); //moze i koristeci li, jos je lakse
    __asm__ volatile ("mv t5, %0" : : "r" (0x5555));
    __asm__ volatile ("sw t5, 0(t6)");
}

void Riscv::handleTimerInterrupt() {
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    mc_sip(SIP_SSIP);

    SleepingQueue::timerQueueUpdate();
    SCB::semaphoreTimerUpdate();

    timeSliceCounter++;
    if (timeSliceCounter >= TCB::running->getTimeSlice()) {
        timeSliceCounter = 0;
        TCB::dispatch();
    }

    w_sstatus(sstatus);
    w_sepc(sepc);
}

void Riscv::_getc() {
    char c = __getc();
    w_stack_a0(c);
}

void Riscv::_putc() {
    char c = r_stack_a1();
    __putc(c);
}

