#ifndef KIRIN_ASYNC_ASYNC_H
#define KIRIN_ASYNC_ASYNC_H

#include "kirin/common/error_def.h"
#include "kirin/common/ref_counter.h"
#include "kirin/job/base_item.h"
#include "kirin/message/message_base.h"

BEGIN_KIRIN_NS(async);

class callbacker;
struct async_work_item: public job::work_item {
    int error;
    union {
        message::message_base* message; /// for complex usage
        int action;
    };
    callbacker* p_callbacker;
    uint64_t n_run_tick;

    async_work_item():
                   work_item(),
                   error(0),
                   message(NULL),
                   p_callbacker(NULL),
                   n_run_tick(0) {}

    explicit async_work_item(int action):
                                    work_item(),
                                    error(0),
                                    action(0),
                                    p_callbacker(NULL),
                                    n_run_tick(0) {
    }
    explicit async_work_item(message::message_base* msg): 
                                    work_item(), 
                                    error(0),
                                    message(msg),
                                    p_callbacker(NULL),
                                    n_run_tick(0) {}

    virtual ~async_work_item() {
    }

    virtual const async_work_item* get_class() const {
        return this;
    }
};

class callbacker: public common::ref_counter<uint32_t> {
public:
    callbacker(callbacker* pob);
    callbacker();
    virtual ~callbacker();

    uint64_t get_id() const {
        return m_id;
    }

    uint32_t get_async_id() const {
        return m_async_id;
    }

    int get_work_thread_id();
    bool is_in_work_thread();
    virtual uint32_t add_ref() {
        return common::ref_counter<uint32_t>::add_ref();
    }

    virtual uint32_t get_ref() {
        return common::ref_counter<uint32_t>::get_ref();
    }

    virtual uint32_t dec_ref() {
        return common::ref_counter<uint32_t>::dec_ref();
    }

    virtual void release() {
        //this->dec_ref();
        if (this->dec_ref() == 0) delete this;
    }

    const char* get_name() const {
        return m_name;
    }

    virtual void callback(async_work_item* item) = 0;

private:
    void init();

protected:
    uint64_t m_id;
    uint32_t m_async_id;
    char m_name[64];
};

END_KIRIN_NS(async);
#endif
