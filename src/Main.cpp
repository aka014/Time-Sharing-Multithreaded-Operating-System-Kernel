#include "../h/Main.hpp"
#include "../h/Riscv.hpp"
#include "../h/MyConsole.hpp"

#include "../test/printing.hpp"

TCB* Main::kernel = nullptr;
TCB* Main::user = nullptr;
TCB* Main::console = nullptr;

extern void userMain();

void mainWrapper(void* arg) {
    userMain();
}


int main() { //trazi da bude int
    MemAllocator::initialize();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    Main::kernel = new TCB(nullptr, nullptr, nullptr, DEFAULT_TIME_SLICE, TCB::SUPERVISOR);
    Main::console = new TCB(MyConsole::consoleWrapper, nullptr, (uint64*) MemAllocator::mem_alloc_argument(DEFAULT_STACK_SIZE),
                            DEFAULT_TIME_SLICE, TCB::SUPERVISOR);
    Main::user = new TCB(mainWrapper, nullptr);

    TCB::running = Main::kernel;

    Scheduler::put(Main::console);
    Scheduler::put(Main::user);


    TCB::running->setBlocked(true);

    // dozvoljava spoljasnje prekide
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    thread_dispatch();

    Riscv::stopEmulator();
    return 0;
}
