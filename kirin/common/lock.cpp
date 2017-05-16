#include "kirin/common/lock.h"
#include "kirin/common/atomic_op.h"

BEGIN_KIRIN_NS(common);

exclusive_lock::exclusive_lock(mutex& mut):
    lock<mutex>(mut) {
    m_mutex.lock();
    ATOMIC_SET(&m_islocked, 1);
}

exclusive_lock::~exclusive_lock() {
    if (is_locked()) unlock();
}


rd_lock::rd_lock(rw_mutex& mut):
                lock<rw_mutex>(mut) {
    m_mutex.rd_lock();
    ATOMIC_SET(&m_islocked, 1); 
}

rd_lock::~rd_lock() {
    if (is_locked()) unlock();
}

wr_lock::wr_lock(rw_mutex& mut):
                 lock<rw_mutex>(mut) {
    m_mutex.wr_lock();
    ATOMIC_SET(&m_islocked, 1);
}

wr_lock::~wr_lock() {
    if (is_locked()) unlock();
}

spin_lock::spin_lock(spin& sp): m_spin(sp) {
    m_spin.lock();
}

spin_lock::~spin_lock() {
    m_spin.unlock();
} 

condition_var::condition_var(mutex& mut):
                     m_mutex(mut) {
    pthread_cond_init(&m_cond, NULL);
}

condition_var::~condition_var() {
    pthread_cond_destroy(&m_cond);
}   

bool condition_var::signal() {
    return pthread_cond_signal(&m_cond) == 0;
}

bool condition_var::wait() {
    return pthread_cond_wait(&m_cond, &m_mutex.get_mutex()) == 0;
}

bool condition_var::broadcast() {
    return pthread_cond_broadcast(&m_cond) == 0;
}
END_KIRIN_NS(common);
