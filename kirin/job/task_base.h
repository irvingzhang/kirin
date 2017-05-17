#ifndef KIRIN_JOB_TASK_BASE_H
#define KIRIN_JOB_TASK_BASE_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(job);

typedef void (*task_func)(void*);

class task_base {
public:
    virtual ~task_base() {}

    virtual void run() = 0;
};

class task: public task_base {
public:
    task(task_func func, void* arg):
            m_func(func),
            m_arg(arg) {}
    virtual ~task() {}

    virtual void run() {
        m_func(m_arg);
    }

protected:
    task_func m_func;
    void* m_arg;
};

END_KIRIN_NS(job);
#endif
