#ifndef KIRIN_JOB_TASK_QUEUE_H
#define KIRIN_JOB_TASK_QUEUE_H

#include <list>
#include <algorithm>
#include "kirin/common/lock.h"

BEGIN_KIRIN_NS(job);

template<typename T>
class task_queue {
public:
    task_queue();
    ~task_queue();

    void produce(T* task, bool is_emergent = false);
    bool replace(T* old_ele, T* new_ele);
    bool consume(T*& task);
    bool contains(T* ele);
    void close();
    inline size_t size() const; 
    bool run();

private:
    volatile bool m_started;
    common::mutex m_mutex;
    common::condition_var m_cond;
    std::list<T*> m_tasks;
};

template<typename T>
task_queue<T>::task_queue(): 
                 m_started(true), 
                 m_cond(m_mutex) {
    m_tasks.clear();
}

template<typename T>
task_queue<T>::~task_queue() {
    m_started = false;
}

template<typename T>
size_t task_queue<T>::size() const {
    return m_tasks.size();
}

template<typename T>
void task_queue<T>::close() {
    common::exclusive_lock lock(m_mutex);
    m_started = false;
    m_cond.broadcast();
}

template<typename T>
bool task_queue<T>::replace(T* old_ele, T* new_ele) {
    common::exclusive_lock lock(m_mutex);
    typename std::list<T*>::iterator it = std::find(m_tasks.begin(), m_tasks.end(), old_ele);
    if (it == m_tasks.end()) return false;

    *it = new_ele;
    return true;
}

template<typename T>
bool task_queue<T>::contains(T* ele) {
    common::exclusive_lock lock(m_mutex);
    typename std::list<T*>::iterator it = std::find(m_tasks.begin(), m_tasks.end(), ele);
    
    return it != m_tasks.end();
}

template<typename T>
void task_queue<T>::produce(T* task, bool is_emergent) {
    common::exclusive_lock lock(m_mutex);
    bool need_signal = m_tasks.empty();
    is_emergent ? m_tasks.push_front(task) :
                 m_tasks.push_back(task);
    
    if (need_signal) m_cond.signal();
}

template<typename T>
bool task_queue<T>::consume(T*& task) {
    common::exclusive_lock lock(m_mutex);
    while (m_tasks.empty()) {
        if (!m_started) return false;

        m_cond.wait();
    }

    task = m_tasks.front();
    m_tasks.pop_front();

    return true;
}

template<typename T>
bool task_queue<T>::run() {
    T* task;
    while (this->consume(task)) {
        task->run();
    }

    return true;
}

END_KIRIN_NS(job);
#endif
