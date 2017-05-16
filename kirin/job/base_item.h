#ifndef KIRIN_JOB_BASE_ITEM_H
#define KIRIN_JOB_BASE_ITEM_H

#include "kirin/common/identifiable.h"
#include "kirin/job/task_queue.h"

BEGIN_KIRIN_NS(job);

struct item_base;
struct work_ctx;
typedef void (*JOB_FUNC)(item_base*);
typedef bool (*CONTROL_JOB_FUNC)(item_base*, work_ctx*);

struct item_base: public common::identifiable {

    virtual ~item_base() {}
    virtual const item_base* get_class() const {
        return this;
    }
};

struct work_item: public item_base {
    JOB_FUNC pfunc;

    virtual ~work_item() {}
    virtual const work_item* get_class() const {
        return this;
    }
};

struct control_work_item: public item_base {
    CONTROL_JOB_FUNC pfunc;

    virtual ~control_work_item() {}
    virtual const control_work_item* get_class() const {
        return this;
    }
};

struct termination_work_item: public control_work_item {
    int id;

    virtual ~termination_work_item() {}
    virtual const termination_work_item* get_class() const {
        return this;
    }
};

typedef task_queue<item_base> job_queue;
struct work_ctx {
    int id; /// identity for worker, such as 0, 1, 2, 3
    thread_type type;  /// thread type, such as storage, network, cpu
    int thread_id;
    volatile int njobs; /// all handled jobs in current queue, may be negative
    bool is_running;
    job_queue* jobq;
};

END_KIRIN_NS(job);
#endif
