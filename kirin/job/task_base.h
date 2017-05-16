#ifndef KIRIN_JOB_TASK_BASE_H
#define KIRIN_JOB_TASK_BASE_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(job);

class task_base {
public:
    virtual ~task_base() {}

    virtual void run() = 0;
};

class task: public task_base {
public:
    explicit task(void* arg):
            m_arg(arg) {}
    virtual ~task() {}

    virtual void run(); 

protected:
    void* m_arg;
};

END_KIRIN_NS(job);
#endif
