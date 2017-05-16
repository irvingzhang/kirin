#include <sys/syscall.h>
#include "kirin/async/async.h"
#include "kirin/common/common.h"
#include "kirin/common/atomic_op.h"
#include "kirin/common/singleton.h"
#include "kirin/manager/work_manager.h"

BEGIN_KIRIN_NS(async);
static volatile int g_async_id = 0;

void callbacker::init() {
    common::ref_counter<uint32_t>::set_ref(0);
    m_id = ATOMIC_INC(&g_async_id, m_id);
    sprintf(m_name, "callbacker#%lu", m_id);
}

callbacker::callbacker() {
    this->init();
    m_async_id = m_id;
}

callbacker::callbacker(callbacker* pobj) {
    this->init();
    m_async_id = pobj ? pobj->m_async_id : m_id;
}

callbacker::~callbacker() {
}

int callbacker::get_work_thread_id() {
    return manager::g_work_manager->get_tid(m_async_id);
}

bool callbacker::is_in_work_thread() {
    return this->get_work_thread_id() == syscall(SYS_gettid);
}

END_KIRIN_NS(async);
