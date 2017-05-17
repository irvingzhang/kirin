#ifndef KIRIN_JOB_THREAD_H
#define KIRIN_JOB_THREAD_H

#include <pthread.h>
#include <list>
#include "kirin/job/task_base.h"

BEGIN_KIRIN_NS(job);

class thread {
static void* thread_run(void* ptr);

public:
    bool create(task* tsk);
    size_t batch_create(task* tsk, size_t num);
    bool join();
    bool join_range(size_t begin_index, size_t end_index);

private:
    std::list<pthread_t> m_threads;
};

END_KIRIN_NS(job);
#endif
