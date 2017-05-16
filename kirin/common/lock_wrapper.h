#ifndef KIRIN_COMMON_LOCK_WRAPPER_H
#define KIRIN_COMMON_LOCK_WRAPPER_H

#include<pthread.h>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);

// MUTual-EXclude Lock
class mutex {
public:
    mutex();
    ~mutex();

    bool lock();
    bool try_lock();
    bool unlock();
    pthread_mutex_t& get_mutex() {
        return m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

// Reader-Writter Lock 
class rw_mutex {
public:
    rw_mutex();
    ~rw_mutex();

    bool rd_lock();
    bool wr_lock();
    bool unlock();

private:
    bool init();

private:
    pthread_rwlock_t m_rwlock;
};

class spin {
public:
    spin();
    ~spin();

    bool lock();
    bool try_lock();
    bool unlock();

private:
    pthread_spinlock_t m_spin;
};

END_KIRIN_NS(common);

#endif
