#ifndef OS1_PROJEKAT_MYCONSOLE_HPP_
#define OS1_PROJEKAT_MYCONSOLE_HPP_

#include "../h/MyBuffer.hpp"

class MyConsole {
public:
    static void consoleWrapper(void* arg);

    static uint64 r_console_status();
    static uint64 r_console_data();
    static void w_console_data(uint64 c);

    //ove fje zovem u switchu
    static void consolePutc();
    static char consoleGetc();

    //salji karaktere konzoli
    static void put();
    //uzimaj karaktere sa konzole
    static void get();

private:
    static MyBuffer* outputBuff;
    static MyBuffer* inputBuff;

};



#endif