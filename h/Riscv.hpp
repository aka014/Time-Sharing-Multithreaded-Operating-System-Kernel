#ifndef OS1_PROJEKAT_RISCV_HPP_
#define OS1_PROJEKAT_RISCV_HPP_

#include "../lib/hw.h"
#include "../h/tcb.hpp"

//static const uint64 ILLEGAL_INSTRUCTION = 0x0000000000000002;
//static const uint64 ILLEGAL_W_ADDRESS = 0x0000000000000007;
//static const uint64 ILLEGAL_R_ADDRESS = 0x0000000000000005;
static const uint64 USER_ECALL = 0x0000000000000008;
static const uint64 SYSTEM_ECALL = 0x0000000000000009;
static const uint64 EXTERNAL_HARDWARE_INT = 0x8000000000000009;
static const uint64 TIMER_INT = 0x8000000000000001;


class Riscv
{
public:

    // pop sstatus.spp and sstatus.spie bits (has to be a non-inline function)
    static void popSppSpie();

    // read register scause
    static uint64 r_scause();

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // read a0 register
    static uint64 r_a0();

    // read sscratch
    static uint64 r_sscratch();

    // write a0 on stack
    static void w_stack_a0(uint64 val);

    // read a0 on stack
    static uint64 r_stack_a0();

    // read a1 on stack
    static uint64 r_stack_a1();

    // read a2 on stack
    static uint64 r_stack_a2();

    // read a3 on stack
    static uint64 r_stack_a3();

    // read a4 on stack
    static uint64 r_stack_a4();

    // read a5 on stack
    static uint64 r_stack_a5();

    // stop emulator
    static void stopEmulator();

    // supervisor trap
    static void supervisorTrap();

private:
    static uint64 timeSliceCounter;

    static void _getc();

    static void _putc();

    // supervisor trap handler
    static void handleSupervisorTrap();

    // syscall handler
    static void handleSyscall();

    // timer handler
    static void handleTimerInterrupt();

    // console handler
    static void handleConsoleInterrupt();

};

inline void Riscv::w_stack_a0(uint64 val) {
    __asm__ volatile ("sd %[val], 8*10(%[addr])" : : [val] "r"(val), [addr] "r"(TCB::running->fp));
}

inline uint64 Riscv::r_stack_a0() {
    uint64 val;
    __asm__ volatile ("ld %[val], 8*10(%[addr])" : [val] "=r"(val) : [addr] "r"(TCB::running->fp));
    return val;
}

inline uint64 Riscv::r_stack_a1() {
    uint64 val;
    __asm__ volatile ("ld %[val], 8*11(%[addr])" : [val] "=r"(val) : [addr] "r"(TCB::running->fp));
    return val;
}

inline uint64 Riscv::r_stack_a2() {
    uint64 val;
    __asm__ volatile ("ld %[val], 8*12(%[addr])" : [val] "=r"(val) : [addr] "r"(TCB::running->fp));
    return val;
}

inline uint64 Riscv::r_stack_a3() {
    uint64 val;
    __asm__ volatile ("ld %[val], 8*13(%[addr])" : [val] "=r"(val) : [addr] "r"(TCB::running->fp));
    return val;
}

inline uint64 Riscv::r_stack_a4() {
    uint64 val;
    __asm__ volatile ("ld %[val], 8*14(%[addr])" : [val] "=r"(val) : [addr] "r"(TCB::running->fp));
    return val;
}

inline uint64 Riscv::r_stack_a5() {
    uint64 val;
    __asm__ volatile ("ld %[val], 8*15(%[addr])" : [val] "=r"(val) : [addr] "r"(TCB::running->fp));
    return val;
}

inline uint64 Riscv::r_sscratch() {
    uint64 volatile reg;
    __asm__ volatile ("csrr %0, sscratch" : "=r" (reg));
    return reg;
}

inline uint64 Riscv::r_a0() {
    uint64 volatile reg;
    __asm__ volatile ("mv %0, a0" : "=r" (reg));
    return reg;
}

inline uint64 Riscv::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

#endif