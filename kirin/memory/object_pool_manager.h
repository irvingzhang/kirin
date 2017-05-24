#ifndef KIRIN_MEMORY_OBJECT_POOL_MANAGER_H
#define KIRIN_MEMORY_OBJECT_POOL_MANAGER_H

#include <list>
#include "kirin/async/async.h"
#include "kirin/memory/object_pool.h"

BEGIN_KIRIN_NS(memory);

class object_pool_manager: public async::callbacker {
public:
    object_pool_manager();
    ~object_pool_manager();

    void do_register(object_pool* p_pool);
    void deregister(object_pool* pool);
    void start_gc();
    virtual void callback(async::async_work_item* item);

private:
    void do_gc();

private:
    volatile bool m_gc_started;
    std::list<object_pool*> m_pools;
};

extern object_pool_manager* g_object_pool_manager;

END_KIRIN_NS(memory);
#endif
