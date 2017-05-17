#include <assert.h>
#include <iostream>
#include "kirin/job/thread.h"

BEGIN_KIRIN_NS(job);

void* thread::thread_run(void* ptr) {
    task* task_ptr = static_cast<task*>(ptr);
    task_ptr->run();

    KIRIN_DELETE_AND_SET_NULL(task_ptr);
    return NULL;
}

bool thread::create(task* tsk) {
    assert(tsk != NULL);
    pthread_t ptid;

    if (::pthread_create(&ptid, NULL,
                    thread_run, tsk) != 0) {
        std::cout << "create thread failed" << std::endl;
        return false;
    }

    m_threads.push_back(ptid);
    return true;
}

size_t thread::batch_create(task* tsk, size_t num) {
    size_t index = 0;
    for (; index < num; ++index) {
        if (!this->create(tsk)) break;
    }

    return index;
}

bool thread::join() {
    for (std::list<pthread_t>::const_iterator it = m_threads.begin();
            it != m_threads.end(); ++it) {
        pthread_join(*it, NULL);
    }

    m_threads.clear();
    return true;
}

bool thread::join_range(size_t begin_index, size_t end_index) {
    size_t actual_begin = std::min(begin_index, end_index);
    size_t actual_end = std::max(begin_index, end_index);

    if (actual_end > m_threads.size()) return false;

    std::list<pthread_t>::iterator it_begin, it_end;
    it_begin = it_end = m_threads.begin();
    std::advance(it_begin, actual_begin);
    std::advance(it_end, actual_end);

    for (; it_begin != it_end;) {
        pthread_join(*it_begin, NULL);
        it_begin = m_threads.erase(it_begin);
    }

    return true;
}

END_KIRIN_NS(job);
