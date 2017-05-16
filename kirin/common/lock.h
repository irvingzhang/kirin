#ifndef KIRIN_COMMON_LOCK_H
#define KIRIN_COMMON_LOCK_H

#include "kirin/common/lock_wrapper.h"
#include "kirin/common/atomic_op.h"

BEGIN_KIRIN_NS(common);

template<typename T>
class lock {
public:
    lock(T& mut): m_mutex(mut) {}
    virtual ~lock() {}
    bool unlock() {
        ATOMIC_SET(&m_islocked, 0);
        return m_mutex.unlock();
    }
    bool is_locked() {
        return ATOMIC_GET(&m_islocked);
    }

protected:
    volatile int m_islocked;
    T& m_mutex;
};

class exclusive_lock: public lock<mutex> {
public:
    explicit exclusive_lock(mutex& mut);
    virtual ~exclusive_lock();
}; 

class rd_lock: public lock<rw_mutex> {
    explicit rd_lock(rw_mutex& mut);
    virtual ~rd_lock();
};

class wr_lock: public lock<rw_mutex> {
public:
    explicit wr_lock(rw_mutex& mut);
    virtual ~wr_lock();
};

class spin_lock {
public:
    spin_lock(spin& sp);
    ~spin_lock();

private:
    spin& m_spin;
};

class condition_var {
public:
    explicit condition_var(mutex& mut);
    ~condition_var();

    bool signal();
    bool wait();
    bool broadcast();

private:
    mutex& m_mutex;
    pthread_cond_t m_cond;
};

END_KIRIN_NS(common);
#endif
