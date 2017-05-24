#include <assert.h>
#include <stdexcept>
#include "kirin/memory/membuf_pool.h"
#include "kirin/memory/memory_op.h"

BEGIN_KIRIN_NS(memory);

membuf_pool::membuf_pool(const char* name): 
                        object_pool(name),
                        m_alloc_times(0),
                        m_args(NULL) {

}

membuf_pool::~membuf_pool() {
    this->destroy();
}

void membuf_pool::destroy(bool is_final) {
    m_freemem.clear();
    if (is_final) KIRIN_DELETE_AND_SET_NULL(m_args);

    for (std::map<char*, buf_handle*>::iterator it = m_free_buffer_handlers.begin();
            it != m_free_buffer_handlers.end(); ++it) {
        ::free(it->first);
        ::free(it->second);
    }

    m_free_buffer_handlers.clear();
}

bool membuf_pool::init(mem_arg* arg) {
    assert(arg != NULL && arg->m_blocksize > 0 && 
           arg->m_per_alloc_count > 0 && 
           arg->m_max_alloc_times > 0 && 
           arg->m_blockbits > 0 && 
           arg->m_low_priority_ratio <= arg->m_high_priority_ratio);

    m_args = arg;
    m_alloc_times = 0;
    for (uint32_t alloc_time = 0; alloc_time < m_args->m_min_alloc_times; 
            ++alloc_time) {
        if (!this->inner_alloc()) return false;
    }

    return true;
}

char* membuf_pool::alloc(uint32_t priority) {
    const uint32_t max_retry_time = std::max(2, (int)(m_args->m_max_alloc_times - m_alloc_times));
    for (size_t retry_time = 0; retry_time < max_retry_time; ++retry_time) {
        if (m_args->m_allocated_count - m_freemem.size() > m_args->m_watermarks[priority]) {
            printf("allocated blocks in priority %d has reached water mark %ld\n", 
                    priority, m_args->m_watermarks[priority]);
    
            return NULL;
        } else if (m_freemem.size() > 0) {
            buf_handle* try_alloc_mem = m_freemem.front();
            m_freemem.pop_front();
            assert(try_alloc_mem != NULL && try_alloc_mem->ref_counter == 0
                   && try_alloc_mem->p_handle_addr = try_alloc_mem);
            ++try_alloc_mem->ref_counter;
            return try_alloc_mem->p_buff;
        } else {
            if (m_args->m_allocated_count >= m_args->m_max_alloc_count || !this->inner_alloc()) {
                return NULL;
            }
        }
    }

    return NULL;
} 

bool membuf_pool::inner_alloc() {
    if (m_alloc_times > m_args->m_max_alloc_times) return false;

    char* p_buf = static_cast<char*>(align_alloc(m_args->m_blockbits, m_args->m_buffsize));
    buf_handle* p_hdl = static_cast<buf_handle*>(memory::alloc(m_args->m_per_alloc_count * sizeof(buf_handle)));
    
    if (p_buf && p_hdl) {
        m_free_buffer_handlers.insert(std::make_pair(p_buf, p_hdl));

        for (uint32_t index = 0; index < m_args->m_per_alloc_count; ++index) {
            p_hdl->p_buff = p_buf;
            p_hdl->ref_counter = 0;
            p_hdl->p_handle_addr = p_hdl;
            m_freemem.push_back(p_hdl);

            p_buf += m_args->m_blocksize;
            ++p_hdl;
        }

        ++m_alloc_times;
        m_args->m_allocated_count = m_alloc_times * m_args->m_per_alloc_count;
        printf("pool %s alloc %d times\n", m_name, m_alloc_times);
        return true;
    }
 
    if (p_buf) ::free(p_buf);
    if (p_hdl) ::free(p_hdl);

    return false;
}

membuf_pool::buf_handle* membuf_pool::get_handle(char* p_buf) {
    std::map<char*, buf_handle*>::iterator it = m_free_buffer_handlers.upper_bound(p_buf);
    if (it != m_free_buffer_handlers.begin()) {
        --it;
    
        char* p_buf_start = it->first;
        assert(p_buf_start <= p_buf);
        int offset = (p_buf - p_buf_start) >> m_args->m_blockbits; /// estimate the block that p_buf falls in
        return it->second + offset;
    }

    return NULL;
}

bool membuf_pool::free(char *p) {
    buf_handle* p_hdl = this->get_handle(p);
    
    if (!p_hdl) throw std::runtime_error("not allocated from memory pool");

    assert(p_hdl == p_hdl->p_handle_addr);

    if (--p_hdl->ref_counter == 0) {
        m_freemem.push_back(p_hdl);
    }

    return true;
}

uint64_t membuf_pool::get_alloc_block_count() {
    return m_args->m_allocated_count;
}

uint64_t membuf_pool::get_free_block_count() {
    return static_cast<uint64_t>(m_freemem.size());
}

void membuf_pool::gc() {
    if (m_args->m_allocated_count == m_freemem.size() && 
            m_args->m_allocated_count != m_args->m_per_alloc_count) {
        printf("start gc for %s\n", m_name);
        this->destroy(false);
        m_alloc_times = 0;
        this->init(m_args);        
    }
}

END_KIRIN_NS(memory);
