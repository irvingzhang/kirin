#ifndef KIRIN_MEMORY_MEMORY_OP_H
#define KIRIN_MEMORY_MEMORY_OP_H

#include <malloc.h>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(memory);
static inline void* alloc(const size_t size) {
    void* ptr = malloc(size);
    if (ptr != NULL) memset(ptr, 0, size);

    return ptr; 
}

template<typename T>
static inline T* alloc() {
    return static_cast<T*>(alloc(sizeof(T)));
}

static inline void* align_alloc(const uint16_t align_bit, const uint64_t total_size) {
    void *ptr = memalign(align_bit, total_size);

    if (ptr != NULL) memset(ptr, 0, total_size);

    return ptr;
}

END_KIRIN_NS(memory);

#endif
