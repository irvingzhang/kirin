#include "kirin/memory/object_pool.h"
#include "kirin/memory/object_pool_manager.h"

BEGIN_KIRIN_NS(memory);

object_pool::object_pool(const char* name) {
    strncpy(m_name, name, sizeof(name));
    g_object_pool_manager->do_register(this);
}

object_pool::~object_pool() {
}

END_KIRIN_NS(memory);
