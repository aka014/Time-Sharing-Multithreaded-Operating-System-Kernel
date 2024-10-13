#ifndef OS1_PROJEKAT_MAIN_HPP_
#define OS1_PROJEKAT_MAIN_HPP_

#include "tcb.hpp"

class Main {
public:
    static TCB* kernel;
    static TCB* user;
    static TCB* console;
};



#endif