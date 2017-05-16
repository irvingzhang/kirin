#ifndef KIRIN_COMMON_ATOMIC_OP_H
#define KIRIN_COMMON_ATOMIC_OP_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);

template<typename T>
inline T atomic_add(volatile T* ptr, T val) {
    __asm__ __volatile__(
        "lock xadd %0, (%1);"
        : "=a"(val)
        : "r"(ptr), "a"(val)
        : "memory"
    );

    return val;
}

template<typename T>
inline T atomic_swap(volatile T* ptr, T val) {
    __asm__ __volatile__(
        "lock xchg %0, (%1);"
        : "=a"(val)
        : "r"(ptr), "a"(val)
        : "memory"
        );

    return val;
}

template<typename T>
inline T atomic_cmp_swap(volatile T* ptr,
                         T exchange,  
                         T comperand) {
    __asm__ __volatile__(
        "lock cmpxchg %1, (%2)"
        :"=a"(comperand)
        :"d"(exchange), "r"(ptr), "a"(comperand) 
        );
    return comperand;
}

#if __GNUC__ > 3
# define ATOMIC_INC(src_ptr, val)          __sync_add_and_fetch(src_ptr, val)
# define ATOMIC_DEC(src_ptr, val)          __sync_sub_and_fetch(src_ptr, val)
# define ATOMIC_SET(src_ptr, val)          __sync_bool_compare_and_swap(src_ptr, *(src_ptr), val)
# define ATOMIC_GET(src_ptr)               __sync_add_and_fetch(src_ptr, 0)
#else
# define ATOMIC_INC(src_ptr, val)          (atomic_add(src_ptr, val) + val)
# define ATOMIC_DEC(src_ptr, val)          (atomic_add(src_ptr, -val) - val)
# define ATOMIC_SET(src_ptr, val)          atomic_swap(src_ptr, val)
# define ATOMIC_GET(src_ptr)               atomic_add(src_ptr, 0)
#endif

END_KIRIN_NS(common);
#endif
