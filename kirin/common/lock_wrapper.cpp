#include <stdexcept>
#include "kirin/common/lock_wrapper.h"

BEGIN_KIRIN_NS(common);

mutex::mutex() {
    pthread_mutex_init(&m_mutex, NULL);
}

mutex::~mutex() {
    pthread_mutex_destroy(&m_mutex);
}

bool mutex::lock() {
    if (pthread_mutex_lock(&m_mutex)) return false;

    return true;
}

bool mutex::try_lock() {
    if (pthread_mutex_trylock(&m_mutex)) return false;

    return true;
}

bool mutex::unlock() {
    if (pthread_mutex_unlock(&m_mutex)) return false;

    return true;
}

bool rw_mutex::init() {
    pthread_rwlockattr_t attrs;
    if (pthread_rwlockattr_init(&attrs)) return false;

    int err = pthread_rwlockattr_setkind_np(&attrs,
                        PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    if (!err) {
        pthread_rwlock_init(&m_rwlock, &attrs);
    }

    pthread_rwlockattr_destroy(&attrs);
    return err == 0;
}

rw_mutex::rw_mutex() {
    if (!init()) throw std::runtime_error("init rwlock error"); 
}

rw_mutex::~rw_mutex() {
    pthread_rwlock_destroy(&m_rwlock);
}

bool rw_mutex::rd_lock() {
    return pthread_rwlock_rdlock(&m_rwlock) == 0;
}

bool rw_mutex::wr_lock() {
    return pthread_rwlock_wrlock(&m_rwlock) == 0;
}

bool rw_mutex::unlock() {
    return pthread_rwlock_unlock(&m_rwlock) == 0;
}

spin::spin() {
    pthread_spin_init(&m_spin, 0);
}

spin::~spin() {
    pthread_spin_destroy(&m_spin);
}

bool spin::lock() {
    return pthread_spin_lock(&m_spin) == 0;
}

bool spin::try_lock() {
    if (pthread_spin_trylock(&m_spin)) return false;

    return true;
}

bool spin::unlock() {
    return pthread_spin_unlock(&m_spin);
}

END_KIRIN_NS(common);
