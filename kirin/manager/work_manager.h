#ifndef KIRIN_MANAGER_WORK_MANAGER_H
#define KIRIN_MANAGER_WORK_MANAGER_H

#include "kirin/timer/timer_service.h"
#include "kirin/async/async.h"
#include "kirin/job/thread_pool.h"

BEGIN_KIRIN_NS(manager);

class work_manager: public timer::timer_service {
public:
    work_manager();
    ~work_manager();

    int start(size_t workers, uint32_t timer_precision);
    void stop(bool wait_pool_stop);

    size_t get_worker_count();
    bool is_running();
    int append_job(async::async_work_item* p_item, 
                   bool is_callbacker_referred, 
                   bool is_emergent = false);
    int delay_run(async::async_work_item* p_item,
                  bool is_callbacker_referred,
                  uint64_t delay_time_in_ms);
    int cancel_run(async::async_work_item* p_item);
    
    int get_tid(uint32_t async_id);

protected:
    virtual int do_process(job::item_base* p_item,
                           bool is_callbacker_referred,
                           bool is_emergent);

private:
    int get_work_queue(async::async_work_item* p_item) const;
    int cancel_queue(async::async_work_item* p_item);

    static void callback(job::item_base*);
    static void dummy(job::item_base* item_ptr);

private:
    job::thread_pool m_thread_pool;
    volatile int m_current_jobs;
};

extern work_manager* g_work_manager;

END_KIRIN_NS(manager);

#endif
