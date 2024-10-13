#include "../h/Queue.hpp"

void Queue::insert(TCB *data) {
    Elem *elem = new Elem(data, nullptr);
    if (tail)
    {
        tail->next = elem;
        tail = elem;
    } else
    {
        head = tail = elem;
    }
}

TCB* Queue::remove() {
    if (!head) { return nullptr; }

    Elem *elem = head;
    head = head->next;
    if (!head) { tail = 0; }

    TCB *ret = elem->data;
    delete elem;
    return ret;
}

TCB* Queue::peekFirst() {
    if (!head) { return nullptr; }
    return head->data;
}

void Queue::insertSleep(TCB *data) {
    Elem* elem = new Elem(data, nullptr);
    if (head) {
        Elem* prev = nullptr;
        Elem* curr = this->head;
        //trazim gde treba ubaciti nit i kako projdme svaki element oduzimam za njegov broj perioda
        while (curr && data->getSleepTime() > curr->data->getSleepTime()) {
            data->setSleepTime(data->getSleepTime() - curr->data->getSleepTime());
            prev = curr;
            curr = curr->next;
        }
        //prevezivanje liste
        elem->next = curr;
        if (!prev) this->head = elem;
        else prev->next = elem;
        //potrebno je i azurirati sleep time elementa iza ovog novododatog
        if (curr) curr->data->setSleepTime(curr->data->getSleepTime() - data->getSleepTime());
    }
    else {
        head = elem;
    }
}

TCB* Queue::removeSleep() {
    if (!this->head) return nullptr;

    Elem* old = this->head;
    TCB* ret = old->data;
    this->head = this->head->next;

    delete old;
    return ret;
}

int Queue::semQueueTimerTick() {
    Elem* curr = this->head;
    int deblockedThreads = 0;
    while (curr) {
        time_t timeout = curr->data->getWaitTime();
        if (timeout == 0) {
            curr->data->setBlocked(false);
            curr->data->setSemaphoreTimeout(true);
            this->removeTCB(curr->data);
            ++deblockedThreads;
            Scheduler::put(curr->data);
        }
        else {
            curr->data->setWaitTime(timeout - 1);
        }
        curr = curr->next;
    }
    return deblockedThreads;
}

void Queue::removeTCB(TCB *tcb) {
    //moram i tail da updateujem
    Elem* curr = this->head;
    Elem* prev = nullptr;
    while (curr && curr->data != tcb) {
        prev = curr;
        curr = curr->next;
    }
    if (!prev) {
        head = head->next;
    }
    if (!curr->next) {
        tail = prev;
    }
    if (!head) tail = nullptr;

    if (prev) prev->next = curr->next;
    delete curr;

}