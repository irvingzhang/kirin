#ifndef KIRIN_JOB_THREAD_POOL_H
#define KIRIN_JOB_THREAD_POOL_H

#include <pthread.h>
#include <vector>
#include "kirin/job/base_item.h"

BEGIN_KIRIN_NS(job);

class thread_pool {
public:
    thread_pool(thread_type type, size_t max_workers, bool share_queue);
    ~thread_pool();

    size_t get_worker_count() const;
    int get_total_job_count(); /// handled and in queue
    int get_tid(const size_t queue_index) const;
    size_t get_processing_jobs(const int queue_index = -1) const; /// all jobs in or waiting processing

    bool start(const size_t workers);
    bool stop();
    bool append_job(item_base* job, int queue_index = -1, bool is_emergent = false);
    bool cancel_job(item_base* job, size_t queue_index);

private:
    size_t add_worker(const size_t workers);
    size_t del_worker(const size_t workers, bool is_final_stop = false);

    static void* thread_func(void* ctx);
    static bool terminate_func(item_base* item, work_ctx* p_ctx);
    static void dummy(item_base* item_ptr);

private:
    volatile int m_jobs;
    bool m_share_queue;
    thread_type m_thread_type;
    size_t m_workers;
    size_t m_max_workers;
    std::vector<pthread_t> m_worker_threads;
    std::vector<work_ctx*> m_ctxs;
};

bool work_ctx_comparator(work_ctx* ctx1, work_ctx* ctx2);

END_KIRIN_NS(job);
#endif
