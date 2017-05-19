#include <sched.h>
#include <iostream>
#include <assert.h>
#include <sys/syscall.h>
#include "kirin/job/thread_pool.h"
#include "kirin/common/atomic_op.h"
#include "kirin/common/cast.h"
#include "kirin/memory/memory_op.h"

BEGIN_KIRIN_NS(job);

void thread_pool::thread_run(void* arg) {
    work_ctx* p_ctx = static_cast<work_ctx*>(arg);
    job_queue* jobq = p_ctx->jobq;
    assert(jobq != NULL);

    p_ctx->thread_id = syscall(SYS_gettid);
    std::cout << "thread start running: " << p_ctx->thread_id << std::endl;
    while (true) {
        item_base* item = NULL;
        if (!jobq->consume(item)) break;
        assert(item != NULL);

        if (is_instance_of<control_work_item>(item)) {
            control_work_item* control_item_ptr = down_cast<control_work_item*>(item);

            if (!control_item_ptr->pfunc(control_item_ptr, p_ctx)) break;
        } else if (is_instance_of<termination_work_item>(item)) {
            termination_work_item* termination_item_ptr = down_cast<termination_work_item*>(item);

            if (!termination_item_ptr->pfunc(termination_item_ptr, p_ctx)) break;
        } else {
            work_item* item_ptr = down_cast<work_item*>(item);
            if (item_ptr->pfunc != NULL) {
                p_ctx->is_running = true;
                item_ptr->pfunc(item_ptr);
                p_ctx->is_running = false;
            }
        }
    }
}

bool work_ctx_comparator(work_ctx* ctx1, work_ctx* ctx2) {
    return ctx1->jobq->size() < ctx2->jobq->size();
}

void thread_pool::dummy(item_base* item_ptr) {
    std::cout << "run dummy" << std::endl;
    KIRIN_DELETE_AND_SET_NULL(item_ptr);
}

bool thread_pool::terminate_func(item_base* item, work_ctx* p_ctx) {
    termination_work_item* ptr = down_cast<termination_work_item*>(item);
    if (ptr->id != -1 && ptr->id != p_ctx->id) {
        p_ctx->jobq->produce(item); /// in shared queue, not current thread to terminate
        sched_yield();
        return true;
    }

    std::cout << "run terminate_func of thread: " << p_ctx->thread_id << std::endl;
    p_ctx->jobq->close();
    return false;
}

thread_pool::thread_pool(thread_type type, 
                         size_t max_workers, 
                         bool share_queue):
              m_jobs(0),
              m_share_queue(share_queue),
              m_thread_type(type),
              m_workers(0),
              m_max_workers(max_workers) {
    m_ctxs.clear();
    for (size_t index = 0; index < m_max_workers; ++index) {
        work_ctx* p_ctx = static_cast<work_ctx*>(alloc(sizeof(work_ctx)));
        p_ctx->id = index;
        p_ctx->type = m_thread_type;
        p_ctx->njobs = 0;
        m_ctxs.push_back(p_ctx);
    }

    if (m_share_queue) {
        m_ctxs[0]->jobq = new (std::nothrow) job_queue;
        assert(m_ctxs[0]->jobq != NULL);
        for (size_t index = 1; index < m_max_workers; ++index) {
            m_ctxs[index]->jobq = m_ctxs[0]->jobq;
        }
    }
}

thread_pool::~thread_pool() {
    this->stop();

    if (m_share_queue) KIRIN_DELETE_AND_SET_NULL(m_ctxs[0]->jobq);

    for (size_t index = 0; index < m_ctxs.size(); ++index) {
        KIRIN_DELETE_AND_SET_NULL(m_ctxs[index]->jobq);
        free(m_ctxs[index]);
    }

    m_ctxs.clear();
}

bool thread_pool::start(const size_t workers) {
    assert(m_workers == 0 && workers > 0 && workers <= m_max_workers);
    const size_t acutal_workers = std::min(workers, m_max_workers);
    return add_worker(acutal_workers) == acutal_workers;
}

bool thread_pool::stop() {
    const size_t origin_workers = m_workers;
    return this->del_worker(m_workers, true) == origin_workers;
}

size_t thread_pool::get_worker_count() const {
    return m_workers;
}

int thread_pool::get_total_job_count() {
    return ATOMIC_GET(&m_jobs);
}

int thread_pool::get_tid(const size_t queue_index) const {
    if (queue_index < 0 || queue_index > m_workers) return -1;

    return m_ctxs[queue_index]->thread_id;
}

size_t thread_pool::get_processing_jobs(const int queue_index) const {
    assert(queue_index >= -1 && queue_index < m_workers);

    if (m_share_queue) return m_ctxs[0]->jobq->size();

    if (queue_index != -1)
        return m_ctxs[queue_index]->jobq->size();

    size_t total = 0;
    for (size_t index = 0; index < m_workers; ++index) 
        total += m_ctxs[index]->jobq->size();

    return total;
}

size_t thread_pool::add_worker(const size_t workers) {
    size_t actual_added = 0;
    for (; actual_added < workers && m_workers < m_max_workers; ++actual_added) {
        if (m_ctxs[m_workers]->jobq == NULL) {
            m_ctxs[m_workers]->jobq = new (std::nothrow) job_queue;
            assert(m_ctxs[m_workers]->jobq != NULL);
        }

        if (m_threads.create(new task(&thread_run, m_ctxs[m_workers]))) ++m_workers;
    }

    return actual_added;
}

size_t thread_pool::del_worker(const size_t workers, bool is_final_stop) {
    termination_work_item* ptr = new (std::nothrow) termination_work_item[workers];
    assert(ptr != NULL);

    size_t actual_del = 0, curr_workers = m_workers;
    for (; actual_del < workers && m_workers > 0; ++actual_del) {
        --m_workers;
        ptr[m_workers].pfunc = terminate_func;
        ptr[m_workers].id = is_final_stop ? -1 : m_workers;
        m_ctxs[m_workers]->jobq->produce(static_cast<item_base*>(ptr + m_workers));
    }

    m_threads.join_range(m_workers, curr_workers);

    delete [] ptr;
    return actual_del;
}

bool thread_pool::append_job(item_base* job, 
                             int queue_index, 
                             bool is_emergent) {
    assert(m_workers > 0);
    if (queue_index < -1 || queue_index > (int)m_workers) return false;

    if (queue_index == -1) {
        work_ctx* ctx = *std::min_element(m_ctxs.begin(), m_ctxs.end(), work_ctx_comparator);
        queue_index = ctx->id;
    } 

    ATOMIC_INC(&m_ctxs[queue_index]->njobs, 1);
    m_ctxs[queue_index]->jobq->produce(job, is_emergent);
    
    return true;
}

bool thread_pool::cancel_job(item_base* job, size_t queue_index) {
    if (queue_index < 0) return false;

    work_item* ptr = new (std::nothrow) work_item();
    assert(ptr != NULL);
    ptr->pfunc = dummy;
    if (m_ctxs[queue_index]->jobq->replace(job, ptr)) return true;

    KIRIN_DELETE_AND_SET_NULL(ptr);
    return false; 
}

END_KIRIN_NS(job);
