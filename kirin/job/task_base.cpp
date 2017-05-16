#include <unistd.h>
#include <sys/syscall.h>
#include <typeinfo>
#include <assert.h>
#include <iostream>
#include "kirin/job/task_base.h"
#include "kirin/job/base_item.h"
#include "kirin/common/cast.h"

BEGIN_KIRIN_NS(job);

void task::run() {
    work_ctx* p_ctx = static_cast<work_ctx*>(m_arg);
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

END_KIRIN_NS(job);
