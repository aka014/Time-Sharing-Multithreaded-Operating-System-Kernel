#include "../h/MyConsole.hpp"
#include "../h/Riscv.hpp"
#include "../h/Main.hpp"
#include "../h/syscall_c.hpp"


MyBuffer* MyConsole::inputBuff = nullptr;
MyBuffer* MyConsole::outputBuff = nullptr;

void MyConsole::consoleWrapper(void *arg) {
    outputBuff = new MyBuffer();
    inputBuff = new MyBuffer();
    put();
}

inline uint64 MyConsole::r_console_status() {
    // CONSOLE_STATUS je adresa gde se ovaj registar velicine 1B nalazi
    uint64 address = CONSOLE_STATUS;
    __asm__ volatile ("mv a6, %0" : : "r"(address));
    __asm__ volatile ("lb a7, 0(a6)");

    uint64 consoleStatus;
    __asm__ volatile ("mv %0, a7" : "=r"(consoleStatus));
    return consoleStatus;
}

inline uint64 MyConsole::r_console_data() {
    // CONSOLE_TX_DATA je adresa gde se ovaj registar velicine 1B nalazi
    uint64 address = CONSOLE_TX_DATA;
    __asm__ volatile ("mv a6, %0" : : "r"(address));
    __asm__ volatile ("lb a7, 0(a6)");

    uint64 c;
    __asm__ volatile ("mv %0, a7" : "=r"(c));
    return c;
}

inline void MyConsole::w_console_data(uint64 c) {
    uint64 address = CONSOLE_RX_DATA;
    __asm__ volatile ("mv a6, %0" : : "r"(address));
    __asm__ volatile ("sb %0, 0(a6)" : : "r"(c));
}

void MyConsole::consolePutc() {
    //stavlja u outputbuff da bi posle Console::put mogao iz njega da uzima
    char c = (char) Riscv::r_stack_a1();
    outputBuff->put(c);
}

char MyConsole::consoleGetc() {
    //uzima iz inputbuff koji se puni iz poziva Console::get
    char c = inputBuff->get();

    if (c == 0xd) c = 0xa; //if (c == CR) c = LF;

    Riscv::w_stack_a0((uint64) c);
    return c;
}

void MyConsole::get() {
    //salje sa konzole u inputbuff
    if (r_console_status() & CONSOLE_RX_STATUS_BIT) {
        char c = (char) r_console_data();
        inputBuff->put(c);
    }
}

void MyConsole::put() {
    //uzima iz outputbuffera i upisuje na konzolu
    //dok god radi user i dok god ima sta da se ispise
    while (!Main::user->isFinished() || !outputBuff->isEmpty()) {
        //dok ima sta da pise i dok je ready bit postavljen
        if (!outputBuff->isEmpty() && (r_console_status() & CONSOLE_TX_STATUS_BIT)) {
            char c = outputBuff->get();
            w_console_data(c);
        }
        else thread_dispatch();
    }

    Scheduler::put(Main::kernel);
    thread_exit();
}