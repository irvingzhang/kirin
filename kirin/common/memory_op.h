#ifndef KIRIN_COMMON_MEMEORY_OP_H
#define KIRIN_COMMON_MEMEORY_OP_H

#include <malloc.h>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);
static inline void* alloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }

    return ptr; 
}

template<typename T>
static inline T* alloc() {
    return static_cast<T*>(alloc(sizeof(T)));
}
END_KIRIN_NS(common);

using kirin::common::alloc;
#endif
