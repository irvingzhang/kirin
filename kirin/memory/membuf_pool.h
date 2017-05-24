#ifndef KIRIN_MEMORY_MEMBUF_POOL_H
#define KIRIN_MEMORY_MEMBUF_POOL_H

#include <list>
#include <map>
#include "kirin/memory/object_pool.h"
#include "kirin/common/arithmetic_util.h"

BEGIN_KIRIN_NS(memory);

struct mem_arg {
    uint32_t m_blocksize, m_per_alloc_count, m_min_alloc_times, m_max_alloc_times;
    uint16_t m_blockbits;
    uint64_t m_buffsize, m_allocated_count, m_max_alloc_count, m_watermarks[3];
    double m_low_priority_ratio, m_high_priority_ratio;

    mem_arg(uint32_t blocksize, uint32_t per_alloc_count,
            uint32_t min_alloc_times, uint32_t max_alloc_times,
            double low_priority_ratio, double high_priority_ratio);
};

class membuf_pool: public object_pool {
public:
    membuf_pool(const char* name);
    ~membuf_pool();

    bool init(mem_arg* arg);
    void destroy(bool is_final = true);
    char* alloc(uint32_t priority);
    bool free(char *p);

    virtual uint64_t get_alloc_block_count();
    virtual uint64_t get_free_block_count();
    virtual void gc();

private:
    struct buf_handle {
        char* p_buff; /// pointer to the actual allocated memory
        buf_handle* p_handle_addr; /// start addr for current handle;
        uint32_t ref_counter;
    
        buf_handle(): p_buff(NULL), 
                      p_handle_addr(NULL), 
                      ref_counter(0) {}
        ~buf_handle() {
            KIRIN_DELETE_AND_SET_NULL(p_buff);
            KIRIN_DELETE_AND_SET_NULL(p_handle_addr);
        }
    };

private:
    bool inner_alloc();
    buf_handle* get_handle(char* p_buf);

private:
    uint32_t m_alloc_times;
    mem_arg* m_args;
    std::list<buf_handle*> m_freemem;
    std::map<char*, buf_handle*> m_free_buffer_handlers; /// save for each buffer, for the purpose of memory free
};

END_KIRIN_NS(memory);
#endif
