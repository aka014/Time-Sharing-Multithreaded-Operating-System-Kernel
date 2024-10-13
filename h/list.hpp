#ifndef OS1_LIST_HPP
#define OS1_LIST_HPP

//!!!ova klasa vise nije u upotrebi, ali nek ostane ovde medju fajlovima!!!

#include "../lib/hw.h"
#include "MemAllocator.hpp"

/*nema potrebe da ima destruktor jer ce se svi elemetni u listi iskoristii vremenom*/

template<typename T>
class List
{
private:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}
    };

    void* operator new(size_t size) {
        return MemAllocator::mem_alloc_argument(size);
    }

    void* operator new[](size_t size) {
        return MemAllocator::mem_alloc_argument(size);
    }

    void operator delete(void* ptr) {
        MemAllocator::mem_free_argument(ptr);
    }

    void operator delete[](void* ptr) {
        MemAllocator::mem_free_argument(ptr);
    }

    Elem *head, *tail;

public:
    List() : head(0), tail(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T *data)
    {
        Elem *elem = new Elem(data, head);
        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data)
    {
        Elem *elem = new Elem(data, 0);
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    // ako nema elemenata vraca 0
    T *removeFirst()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }
};

#endif