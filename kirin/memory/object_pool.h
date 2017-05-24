#ifndef KIRIN_MEMORY_OBJECT_POOL_H
#define KIRIN_MEMORY_OBJECT_POOL_H

#include <stdint.h>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(memory);

class object_pool {
public:
    object_pool(const char* name);
    virtual ~object_pool();

    const char* get_name() const {
        return m_name;
    }
    virtual uint64_t get_alloc_block_count() = 0;
    virtual uint64_t get_free_block_count() = 0;
    virtual void gc() = 0;

protected:
    char m_name[256];
};

END_KIRIN_NS(memory);
#endif
