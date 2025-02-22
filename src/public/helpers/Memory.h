#pragma once
#include "Logging.h"


struct CAllocator {

    void* allocation;
    CAllocator* next;
};

class Memory {
public:

    static Memory* GetInstance() {
        static Memory* instance = new Memory();
        return instance;
    }

    void* Alloc(void* data, size_t size);

    // TODO
    void Free(void* allocation);

    void Cleanup();

private:
    CAllocator* head;
};

