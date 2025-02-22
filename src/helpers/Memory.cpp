#include "../public/helpers/Memory.h"

void* Memory::Alloc(void* data, size_t size) {
    if (!data || size == 0) {
        SDL_Log("Invalid data or size for allocation.");
        return nullptr;
    }

    CAllocator* allocator = (CAllocator*) SDL_malloc(sizeof(CAllocator));
    if (!allocator) {
        LOG_ERROR("Failed to allocate memory for CAllocator.");
        return nullptr;
    }

    allocator->allocation = SDL_malloc(size);
    if (!allocator->allocation) {
        LOG_ERROR("Failed to allocate memory for data.");
        SDL_free(allocator);
        return nullptr;
    }

    SDL_memcpy(allocator->allocation, data, size);
    allocator->next = head;
    head            = allocator;

    return allocator->allocation;
}

void Memory::Cleanup() {


    while (head) {
        auto* next = head->next;
        SDL_free(head);
        head = next;
    }

    LOG_INFO("Memory cleanup complete.");
}
