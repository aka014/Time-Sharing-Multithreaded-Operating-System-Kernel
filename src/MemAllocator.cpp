#include "../h/MemAllocator.hpp"

#include "../h/Riscv.hpp"


MemAllocator MemAllocator::myMemoryAllocator = MemAllocator();
MemAllocator::FreeMem* MemAllocator::freeMemHead = (FreeMem*) HEAP_START_ADDR;
MemAllocator::FreeMem* MemAllocator::usedMemHead = nullptr;


void* MemAllocator::mem_alloc_argument(size_t size_in_bytes) {
    // konverzija u blokove
    size_t size = (size_in_bytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    // u slucaju da se desi neka neregularna situacija, sigurno ce se poslati nullptr
    void* address = nullptr;

    if (size <= 0) return nullptr;

    size_t newSize = size * MEM_BLOCK_SIZE; //size se zadaje u blokovima zbog ABI

    for (FreeMem* curr = freeMemHead; curr != nullptr; curr = curr->next)
    {
        if (curr->size < newSize) continue;
        if (curr->size > newSize)
        {
            //sredjivanje ulancane liste slboodnih segmenata
            // prvo treba na curr dodati velicinu zaglavlja + koliko se mem zauziam, tj. toliko se memorije zauzima
            FreeMem* newBlock = (FreeMem*)((char*) curr + newSize + sizeof(FreeMem));
            //klasicne stvari kod dvostruko ulancanih lista
            if (curr->prev) curr->prev->next = newBlock;
            else freeMemHead = newBlock;
            if (curr->next) curr->next->prev = newBlock;
            newBlock->prev = curr->prev;
            newBlock->next = curr->next;

            newBlock->size = curr->size - newSize - sizeof(FreeMem);
            curr->size = newSize;
            //i tako sad ovaj curr postaje zauzet pa se i izbacuje iz evidencije

            //sredjvanje liste zauzetih segmenata
            if (usedMemHead == nullptr)
            {
                usedMemHead = curr;
                usedMemHead->next = nullptr;
                usedMemHead->prev = nullptr;
            }
                //ako se curr nalazi pre provg elementa u listi zauzetih segmenata
            else if (curr < usedMemHead)
            {
                usedMemHead->prev = curr;
                curr->prev = nullptr;
                curr->next = usedMemHead;
                usedMemHead = curr;
            }
                //potrebno je smestiti ga negde u listi
            else {
                FreeMem* used = usedMemHead;
                for (; used->next && (used->next) < curr; used = used->next);
                curr->next = used->next;
                curr->prev = used;
                if (curr->next) curr->next->prev = curr;
                used->next = curr;
            }
        }
        else //iste su velicine, tj. alocira se ceo segment
        {
            //sredjivanje ulancane liste slboodnih segmenata
            if (curr->prev) curr->prev->next = curr->next;
            else freeMemHead = curr->next;
            if (curr->next) curr->next->prev = curr->prev;

            //sredjvanje liste zauzetih segmenata
            if (usedMemHead == nullptr)
            {
                usedMemHead = curr;
                usedMemHead->next = nullptr;
                usedMemHead->prev = nullptr;
            }
                //curr treba da postane usedMemHead
            else if (curr < usedMemHead) //ovde nisam koristio ikakvu konv mislim da ne treba
            {
                usedMemHead->prev = curr;
                curr->prev = nullptr;
                curr->next = usedMemHead;
                usedMemHead = curr;
            }
                //treba smestiti curr negde u listi zauzetih
            else
            {
                FreeMem* used = usedMemHead;
                for(; used->next && (used->next) < curr; used = used->next);
                curr->next = used->next;
                curr->prev = used;
                if (curr->next) curr->next->prev = curr;
                used->next = curr;
            }
        }
        // ako je regularan mem alloc onda ce se ovde opet u a0 upisati dodeljena adresa
        address = (void*)((char*)curr + sizeof(FreeMem));

        return address;
    }

    return nullptr; //nema dovoljno slobodne memorije
}

int MemAllocator::mem_free_argument(void *ptr) {


    if (ptr == nullptr || ptr < HEAP_START_ADDR || ptr > HEAP_END_ADDR) {
        return -1; //pokazivac ne ukazuje na mesto na heapu
    }
    if (usedMemHead == nullptr) {
        return -2; //nista jos nije alocirano, pa je sigurno greska
    }

    FreeMem* curr = (FreeMem*)((char*)ptr - sizeof(FreeMem)); //alloc vrati na slobdan prostor, a header je ovde


    for (FreeMem* current = usedMemHead; current != nullptr; current = current->next) {
        if (current == curr) break;
        if (current->next == nullptr) {
            return -3;
        }
    }


    if (curr < usedMemHead) {
        return -4; //alocirani segment ne moze btii pre usedMemHead
    }

    // izbacivanje iz liste zauzetih
    if (curr == usedMemHead)
    {
        usedMemHead = curr->next;
        if (usedMemHead) usedMemHead->prev = nullptr;
        curr->next = nullptr;//najsigurnije mi je da ovde stavim nullptr pa cu pri ubacivanju eruglisati pokazivace
        curr->prev = nullptr;
    }
    else
    {
        curr->prev->next = curr->next;
        if (curr->next) curr->next->prev = curr->prev;
        curr->next = nullptr;//najsigurnije mi je da ovde stavim nullptr pa cu pri ubacivanju eruglisati pokazivace
        curr->prev = nullptr;
    }

    //sredjivanje liste slobodnih segmenata
    if (freeMemHead == nullptr)
    {
        freeMemHead = curr; //pokazivaci su vec na vmestu
    }
        //ako je curr ispred freeMemHead
    else if (curr < freeMemHead)
    {
        freeMemHead->prev = curr;
        curr->next = freeMemHead;
        freeMemHead = curr;
        tryToJoin(freeMemHead); //ako se prvobitni curr i freeMemHead dodieuju
    }
        //naci curr mesto u listi
    else
    {
        FreeMem* free = freeMemHead;
        for (; free->next && free->next < curr; free = free->next);
        curr->next = free->next;
        curr->prev = free;
        if (curr->next) curr->next->prev = curr;
        free->next = curr;
        tryToJoin(curr);//bitno je da prvo ide curr jer spaja nadesno
        tryToJoin(free);
    }

    return 0;
}


void MemAllocator::initialize() {
    freeMemHead = (FreeMem*) (HEAP_START_ADDR);
    freeMemHead->next = freeMemHead->prev = nullptr;
    freeMemHead->size = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - sizeof(FreeMem);
    usedMemHead = nullptr;
}

MemAllocator* MemAllocator::get() {
    return &myMemoryAllocator;
}


void* MemAllocator::mem_alloc() {
    // kupim argument f-je
    size_t size = (size_t) Riscv::r_stack_a1();

    // u slucaju da se desi neka neregularna situacija, sigurno ce se poslati nullptr
    void* address = nullptr;
    Riscv::w_stack_a0((uint64) address);

    if (size <= 0) return nullptr;

    size_t newSize = size * MEM_BLOCK_SIZE; //size se zadaje u blokovima zbog ABI

    address = mem_alloc_argument(newSize);
    Riscv::w_stack_a0((uint64) address);
    return address;
}


int MemAllocator::mem_free() {
    void* ptr = (void*) Riscv::r_stack_a1();
    // prenos argumenta

    int retval = mem_free_argument(ptr);

    Riscv::w_stack_a0((uint64) retval);
    return retval;
}

void MemAllocator::tryToJoin(FreeMem* curr) {
    if (!curr) return;
    if (curr->next && (char*)curr + curr->size + sizeof(FreeMem) == (char*)(curr->next))
    {//postoje 2 slboodna segmenta koja se dodiruju pa ih treba spojijti
        curr->size += (curr->next->size + sizeof(FreeMem));
        curr->next = curr->next->next;
        if (curr->next) curr->next->prev = curr;
    }

}
