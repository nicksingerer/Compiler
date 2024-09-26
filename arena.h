#pragma once

#include <cstdlib>
#include <new>
#include <iostream>
#include <memory>

class ArenaAllocator {
public:

    inline explicit ArenaAllocator(size_t bytes) : size(bytes) {

        buffer = static_cast<byte*>(std::malloc(size));

        if (!buffer) {
            cerr << "Memory allocation failed!" << endl;
            exit(EXIT_FAILURE);
        }

        offset = buffer;

    }

    // Deleted copy constructor and assignment operator to prevent copying
    inline ArenaAllocator(const ArenaAllocator& other) = delete;
    inline ArenaAllocator& operator=(const ArenaAllocator& other) = delete;

    inline ~ArenaAllocator() { // Destructor
        free(buffer);
    }

    // Allocate memory for a type T with proper alignment
    template <typename T>
    inline T* allocate() {
        std::size_t alignment = alignof(T);
        std::size_t space = size - (offset - buffer);

        void* aligned_ptr = offset;

        if (align(alignment, sizeof(T), aligned_ptr, space)) {
            offset = static_cast<byte*>(aligned_ptr) + sizeof(T);
            return static_cast<T*>(aligned_ptr);
        } else {
            cerr << "ArenaAllocator: Out of memory or alignment error!" << endl;
            exit(EXIT_FAILURE);
        }
    }

private:
    size_t size;    // Size of the buffer
    byte* buffer;   // Pointer to the buffer
    byte* offset;   // Pointer to the current offset
};
