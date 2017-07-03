#include <assert.h>
#include "kirin/common/singleton.h"
#include "kirin/memory/object_pool_manager.h"
#include "kirin/manager/work_manager.h"
#include "kirin/message/internal_message.h"

BEGIN_KIRIN_NS(memory);

object_pool_manager* g_object_pool_manager = 
        common::singleton<object_pool_manager>::instance_ptr();

object_pool_manager::object_pool_manager() {
    m_async = new (std::nothrow) async::async_work_item;
    assert(m_async != NULL);
    add_ref();
}

void object_pool_manager::start_gc() {
    m_gc_started = true;

    m_async->action = message::internal_action::KIRIN_IA_GC;
    m_async->p_callbacker = this;
    manager::g_work_manager->delay_run(m_async, false, GC_INTERVAL);
    printf("object pools starts gc\n");
}

void object_pool_manager::stop_gc() {
    m_gc_started = false;
     
    manager::g_work_manager->cancel_run(m_async);
    printf("object pools stopped\n");
}

object_pool_manager::~object_pool_manager() {
    if (m_gc_started) stop_gc();

    KIRIN_DELETE_AND_SET_NULL(m_async);

    dec_ref();
}

void object_pool_manager::do_register(object_pool* p_pool) {
    printf("register object pool: %s\n", p_pool->get_name());
    m_pools.push_back(p_pool);
    //if (!m_gc_started) g_object_pool_manager->start_gc();
}

void object_pool_manager::deregister(object_pool* p_pool) {
    printf("deregister object pool: %s\n", p_pool->get_name());
    m_pools.remove(p_pool);
}

void object_pool_manager::callback(async::async_work_item* item) {
    assert(item != NULL);
    if (!m_gc_started) {
        KIRIN_DELETE_AND_SET_NULL(item);
        return;
    }

    switch (item->action) {
        case message::internal_action::KIRIN_IA_GC:
            do_gc();
            manager::g_work_manager->delay_run(item, false, GC_INTERVAL);
            break;
        default:
            printf("unsuported operation: %d\n", item->action);
            KIRIN_DELETE_AND_SET_NULL(item);
    }
}

void object_pool_manager::do_gc() {  // TODO: divide gc into minor/major/full
    for (std::list<object_pool*>::const_iterator it = m_pools.begin();
            it != m_pools.end(); ++it) {
        (*it)->gc();
    }
}

END_KIRIN_NS(memory);
