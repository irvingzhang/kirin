#ifndef KIRIN_COMMON_SLIDING_WINDOW_H
#define KIRIN_COMMON_SLIDING_WINDOW_H

#include <assert.h>
#include <sys/syscall.h>
#include "kirin/common/cast.h"
#include "kirin/memory/memory_op.h"

BEGIN_KIRIN_NS(common);

template<typename ITEM> 
class sliding_window {
public:
    typedef void (*ON_INIT_ITEM)(ITEM* p_item, void* p_ctx);
    typedef void (*ON_SLOT_EMPTY)(ITEM* p_item, void* p_ctx);
    typedef void (*ON_ITEM_READY)(ITEM* p_item, void* p_ctx);

public:
    sliding_window();
    ~sliding_window();

    bool init(uint32_t window_len, ON_INIT_ITEM f_init, ON_SLOT_EMPTY f_empty, ON_ITEM_READY f_ready);
    void start(void* p_ctx);
    void pause();
    void add_item(ITEM* p_item);
    void free_item(ITEM* p_item);

private:
    void slide();

private:
    enum item_state {
        IS_SET_EMPTY, IS_SET_READY, IS_SLIDE_EMPTY, IS_SLIDE_READY,
    };

    enum window_state {
        SWS_NOT_READY, SWS_INIT, SWS_RUNNING, SWS_PAUSE,
    } m_state;

    struct actual_sliding_window_working_item: public ITEM {
        int sliding_window_inner_slot;

        virtual ~actual_sliding_window_working_item() {}
    };
    actual_sliding_window_working_item* m_item_ptr;
    uint8_t* m_byte_map_ptr;
    void* m_ctx;
    int m_thread_id;
    uint32_t m_window_len, m_empty_slot_index, m_ready_slot_index;
    ON_INIT_ITEM m_init_func;
    ON_SLOT_EMPTY m_slot_empty_func;
    ON_ITEM_READY m_item_ready_func;
};

template<typename ITEM>
sliding_window<ITEM>::sliding_window(): 
                        m_state(SWS_NOT_READY),
                        m_item_ptr(NULL),
                        m_byte_map_ptr(NULL),
                        m_ctx(NULL),
                        m_thread_id(-1),
                        m_window_len(0),
                        m_empty_slot_index(0),
                        m_ready_slot_index(0),
                        m_init_func(NULL),
                        m_slot_empty_func(NULL),
                        m_item_ready_func(NULL) {
}

template<typename ITEM>
sliding_window<ITEM>::~sliding_window() {
    KIRIN_ARRAY_DELETE_AND_SET_NULL(m_item_ptr);
    KIRIN_FREE_AND_SET_NULL(m_byte_map_ptr);
}

template<typename ITEM>
bool sliding_window<ITEM>::init(uint32_t window_len, ON_INIT_ITEM f_init, 
                           ON_SLOT_EMPTY f_empty, ON_ITEM_READY f_ready) {
    assert(window_len != 0 && m_state == SW_NOT_READY && m_window_len == 0);
   
    m_window_len = window_len;
    m_item_ptr = new (std::nothrow) actual_sliding_window_working_item[m_window_len];
    assert(m_item_ptr != NULL);
    m_byte_map_ptr = static_cast<uint8_t*>(memory::alloc(m_window_len));

    if (m_item_ptr && m_byte_map_ptr) {
        m_init_func = f_init;
        m_slot_empty_func = f_empty;
        m_item_ready_func = f_ready;
        m_state = SWS_INIT;
        m_empty_slot_index = m_ready_slot_index = 0;
        return true;    
    }

    if (m_item_ptr) KIRIN_ARRAY_DELETE_AND_SET_NULL(m_item_ptr);
    if (m_byte_map_ptr) KIRIN_FREE_AND_SET_NULL(m_byte_map_ptr);

    return false;
}

template<typename ITEM>
void sliding_window<ITEM>::start(void* p_ctx) {
    assert(m_state != SWS_NOT_READY);

    if (m_state != SWS_PAUSE) {
        m_thread_id = syscall(SYS_gettid);
        memset(m_byte_map_ptr, 0, m_window_len);
        m_empty_slot_index = m_ready_slot_index = 0;
        m_ctx = p_ctx;

        if (m_state == SWS_INIT) {
            for (uint32_t index = 0; index < m_window_len; ++index) {
                m_init_func(&m_item_ptr[index], m_ctx);
                m_item_ptr[index].sliding_window_inner_slot = index;
            }
        }
    }

    m_state = SWS_RUNNING;
    this->slide();
}

template<typename ITEM>
void sliding_window<ITEM>::pause() {
    assert(m_thread_id == syscall(SYS_gettid));
    m_state = SWS_PAUSE;
}

template<typename ITEM>
void sliding_window<ITEM>::add_item(ITEM* p_item) {
    assert(p_item != NULL);
    actual_sliding_window_working_item* p_actual_item = down_cast<actual_sliding_window_working_item*>(p_item);    

    assert(m_thread_id == syscall(SYS_gettid));
    #ifdef KIRIN_UNITTEST
        printf("[add_item] m_thread_id: %d, syscall(SYS_gettid): %ld\n", m_thread_id, syscall(SYS_gettid));
    #endif
    assert(m_byte_map_ptr[p_actual_item->sliding_window_inner_slot] == IS_SLIDE_EMPTY);

    m_byte_map_ptr[p_actual_item->sliding_window_inner_slot] = IS_SET_READY;

    if (m_state == SWS_RUNNING) this->slide();
}

template<typename ITEM>
void sliding_window<ITEM>::free_item(ITEM* p_item) {
    assert(p_item != NULL);
    actual_sliding_window_working_item* p_actual_item = down_cast<actual_sliding_window_working_item*>(p_item);

    assert(m_thread_id == syscall(SYS_gettid));
    #ifdef KIRIN_UNITTEST
        printf("[free_item] m_thread_id: %d, syscall(SYS_gettid): %ld\n", m_thread_id, syscall(SYS_gettid));
    #endif
    assert(m_byte_map_ptr[p_actual_item->sliding_window_inner_slot] == IS_SLIDE_READY);
   
    m_byte_map_ptr[p_actual_item->sliding_window_inner_slot] = IS_SET_EMPTY;
    if (m_state == SWS_RUNNING) this->slide();
}

template<typename ITEM>
void sliding_window<ITEM>::slide() {
    while (m_byte_map_ptr[m_ready_slot_index] == IS_SET_READY) {
        #ifdef KIRIN_UNITTEST
            printf("current ready slide index: %d\n", m_ready_slot_index);
        #endif
        m_byte_map_ptr[m_ready_slot_index] = IS_SLIDE_READY;
        m_item_ready_func(&m_item_ptr[m_ready_slot_index], m_ctx);

        m_ready_slot_index = ++m_ready_slot_index % m_window_len;
    }

    while (m_byte_map_ptr[m_empty_slot_index] == IS_SET_EMPTY) {
        #ifdef KIRIN_UNITTEST
            printf("current empty slide index: %d\n", m_empty_slot_index);
        #endif
        m_byte_map_ptr[m_empty_slot_index] =IS_SLIDE_EMPTY;
        m_slot_empty_func(&m_item_ptr[m_empty_slot_index], m_ctx);

        m_empty_slot_index = ++m_empty_slot_index % m_window_len;
    }
}

END_KIRIN_NS(common);
#endif
