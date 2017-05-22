#ifndef KIRIN_MEMORY_OBJECT_POOL_H
#define KIRIN_MEMORY_OBJECT_POOL_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(memory);

class object_pool {
public:
    object_pool(const char* name);
    virtual ~object_pool();

    virtual size_t get_alloc_block_count() = 0;
    virtual size_t get_free_block_count() = 0;
    virtual void gc() = 0;

private:
    char m_name[64];
};

END_KIRIN_NS(memory);
#endif
