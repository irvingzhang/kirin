#ifndef KIRIN_COMMON_REF_COUNTER_H
#define KIRIN_COMMON_REF_COUNTER_H

#include "kirin/common/atomic_op.h"

BEGIN_KIRIN_NS(common);

/// T could be an integer with type int, long unsigned long, etc.
template<typename T>
class ref_counter {
public:
    ref_counter(): m_ref_num(0) {}
    ~ref_counter() {}

    T add_ref(const T n = 1) {
        return ATOMIC_INC(&m_ref_num, n);
    }

    T dec_ref(const T n = 1) {
        return ATOMIC_DEC(&m_ref_num, n);
    }

    T get_ref() {  // Note: don't add "const" qualifier here
        return ATOMIC_GET(&m_ref_num);
    }

    void set_ref(const T n) {
        ATOMIC_SET(&m_ref_num, n);
    }

private:
    volatile T m_ref_num;
};

END_KIRIN_NS(common);
#endif
