#include <climits>
#include <sys/syscall.h>
#include "kirin/common/singleton.h" 
#include "kirin/common/error_def.h"
#include "kirin/common/atomic_op.h"
#include "kirin/common/cast.h"
#include "kirin/timer/timer.h"
#include "kirin/manager/work_manager.h"

BEGIN_KIRIN_NS(manager);

work_manager* g_work_manager = common::singleton<work_manager>::instance_ptr();

void work_manager::callback(job::item_base* p_item) {
    async::async_work_item* p_work_item = down_cast<async::async_work_item*>(p_item);
    async::callbacker* callbacker = p_work_item->p_callbacker;

    p_work_item->pfunc = NULL;
    callbacker->callback(p_work_item);
    callbacker->release();
}

void work_manager::dummy(job::item_base* item_ptr) {

}

work_manager::work_manager():
                m_thread_pool(TT_CPU, MAX_WORKER_THREADS, false) {
    m_current_jobs = INT_MIN;
}

work_manager::~work_manager() {
    this->stop(true);
}

int work_manager::start(size_t workers, uint32_t timer_precision) {
    timer::g_tick_precision = std::min(timer_precision, static_cast<uint32_t>(1000));
    timer::g_last_tick = 0;
    timer::g_cycle_start = timer::rdtsc();
    timer::g_cycle_in_tick = timer::get_cpu_frequency() * timer::g_tick_precision / 1000; /// how many cpu cycles in per tick

    if (m_thread_pool.start(workers)) {
        ATOMIC_SET(&m_current_jobs, 0);
        return common::kirin_error::KIRIN_ERR_OK;
    }

    return common::kirin_error::KIRIN_ERR_SYS;
}

void work_manager::stop(bool wait_pool_stop) {
    if (wait_pool_stop) {
        for (size_t index = 0; index < m_thread_pool.get_worker_count(); ++index) {
            if (m_thread_pool.get_tid(index) == syscall(SYS_gettid)) return; /// do not work in pool threads

            int num = 0;
            while ((num = common::atomic_cmp_swap(&m_current_jobs, INT_MIN, 0)) != 0) {
                return;
            }
            sleep(1);
        }

        m_thread_pool.stop();
    } else {
        ATOMIC_SET(&m_current_jobs, INT_MIN);
    }
}

bool work_manager::is_running() {
    return ATOMIC_GET(&m_current_jobs) >= 0;
}

size_t work_manager::get_worker_count() {
    assert(this->is_running());
    return m_thread_pool.get_worker_count();
}

int work_manager::append_job(async::async_work_item* p_item,
                             bool is_callbacker_referred,
                             bool is_emergent) {
    if (p_item == NULL || p_item->message == NULL || p_item->p_callbacker == NULL)
        return common::kirin_error::KIRIN_ERR_INVALID_ARGUMENTS;

    if (ATOMIC_INC(&m_current_jobs, 1) >= 0) {
        if (!is_callbacker_referred) p_item->p_callbacker->add_ref();
    
        p_item->pfunc = callback;
        m_thread_pool.append_job(static_cast<job::item_base*>(p_item), this->get_work_queue(p_item));
        ATOMIC_DEC(&m_current_jobs, 1);
        return common::kirin_error::KIRIN_ERR_OK;
    }

    if (is_callbacker_referred) p_item->p_callbacker->release();
    ATOMIC_SET(&m_current_jobs, INT_MIN);

    return common::kirin_error::KIRIN_ERR_NO_THREAD_WORKER;
}

int work_manager::delay_run(async::async_work_item* p_item,
                            bool is_callbacker_referred,
                            uint64_t delay_time_in_ms) {
    if (p_item == NULL || p_item->message == NULL || p_item->p_callbacker == NULL)
        return common::kirin_error::KIRIN_ERR_INVALID_ARGUMENTS;

    p_item->pfunc = dummy;
    if (timer::g_tick_precision <= delay_time_in_ms) {
        p_item->p_callbacker->add_ref();

        uint64_t deadline_tick = timer::g_last_tick + delay_time_in_ms / timer::g_tick_precision;
        p_item->n_run_tick = deadline_tick;

        if (add_to_timer(deadline_tick, static_cast<job::item_base*>(p_item))) 
            return common::kirin_error::KIRIN_ERR_OK;
    }

    return append_job(p_item, false);
}

int work_manager::cancel_run(async::async_work_item* p_item) {
    if (p_item->pfunc == dummy) { /// cancel delay running job
        if (remove_from_timer(p_item)) {
            p_item->pfunc = NULL;
            p_item->p_callbacker->release();
            return common::kirin_error::KIRIN_ERR_OK;
        } else {
            return this->cancel_queue(p_item);
        }

    } else if (p_item->pfunc == callback) {  /// cancel normal job
        return this->cancel_queue(p_item);
    }

    return common::kirin_error::KIRIN_ERR_INVALID_ARGUMENTS;
}

int work_manager::cancel_queue(async::async_work_item* p_item) {
    int queue_index =  this->get_work_queue(p_item);
    if (m_thread_pool.cancel_job(p_item, queue_index)) {
        p_item->pfunc = NULL;
        p_item->p_callbacker->release();
        return common::kirin_error::KIRIN_ERR_OK;
    }

    return common::kirin_error::KIRIN_ERR_ALREADY_EXECUTE;
}

int work_manager::do_process(job::item_base* p_item,
                             bool is_callbacker_referred,
                             bool is_emergent) {
    async::async_work_item* p_actual = down_cast<async::async_work_item*>(p_item);
    return append_job(p_actual, is_callbacker_referred, is_emergent);
}

int work_manager::get_work_queue(async::async_work_item* p_item) const {
    assert(p_item != NULL && p_item->p_callbacker != NULL);
    return p_item->p_callbacker->get_async_id() % m_thread_pool.get_worker_count();
}

int work_manager::get_tid(uint32_t async_id) {
    assert(this->is_running());
    async_id %= m_thread_pool.get_worker_count();

    return m_thread_pool.get_tid(async_id);
}

END_KIRIN_NS(manager);
