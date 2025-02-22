#include "../public/helpers/Memory.h"

void* Memory::Alloc(void* data, size_t size) {
    if (!data || size == 0) {
        SDL_Log("Invalid data or size for allocation.");
        return nullptr;
    }

    CAllocator* allocator = (CAllocator*)SDL_malloc(sizeof(CAllocator) + size);
    if (!allocator) {
        LOG_ERROR("Failed to allocate memory for CAllocator.");
        return nullptr;
    }

    allocator->allocation = (void*)(allocator + 1);  
    SDL_memcpy(allocator->allocation, data, size);

    allocator->next = head;
    head = allocator;

    return allocator->allocation;
}

void Memory::Cleanup() {


    while (head) {
        auto* next = head->next;

        if (head->allocation) {
            SDL_free(head->allocation);  
        }

        SDL_free(head);
        head = next;
    }

    LOG_INFO("Memory cleanup complete.");
}
