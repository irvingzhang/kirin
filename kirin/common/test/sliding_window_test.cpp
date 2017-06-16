#ifndef KIRIN_UNITTEST
#define KIRIN_UNITTEST

#include <iostream>
#include <sys/syscall.h>
#include "kirin/async/async.h"
#include "kirin/manager/work_manager.h"
#include "kirin/message/internal_message.h"
#include "kirin/common/test/sliding_window_test.h"
#include "kirin/common/lock.h"

BEGIN_KIRIN_NS(common);
CPPUNIT_TEST_SUITE_REGISTRATION(sliding_window_test);

sliding_window_test::sliding_window_test() {
}

sliding_window_test::~sliding_window_test() {
}

void sliding_window_test::setUp() {
    std::cout << "setup success" << std::endl;
}

void sliding_window_test::tearDown() {
    std::cout << "tearDown success" << std::endl;
}

struct sliding_item {
    int id;
};

struct test_async_item: public async::async_work_item {
    sliding_item* p_item;
    void* p_ctx;

    virtual ~test_async_item() {}

    virtual const test_async_item* get_class() const {
        return this;
    }
};

class action_callbacker: public async::callbacker {
public:
    void callback(async::async_work_item* aitem);

};

class window_test {
public:
    bool init() {
        return sw.init(10, on_init, on_empty, on_ready);
    }

    void start() {
        sw.start(this);
    }

    void run(sliding_item* p_item) {
        common::exclusive_lock lock(m_mutex);
        this->sw.free_item(p_item);
    }

private:
    static void on_init(sliding_item* p_item, void* ptr) {
        memset(p_item, 0, sizeof(sliding_item));
        std::cout << "successfully on_init" << std::endl;
    }

    static void on_empty(sliding_item* p_item, void* ptr) {
        window_test* p_ctx = static_cast<window_test*>(ptr);
        static int counter = 0;
        p_item->id = ++counter;
        std::cout << "on_empty, counter " << counter << std::endl;
        if (counter <= 30)
            p_ctx->sw.add_item(p_item);
    }

    static void on_ready(sliding_item* p_item, void* ptr) {
        int rand_num = (int)(1 + 2.0 * rand() / RAND_MAX);
        std::cout << "on_ready, counter " << p_item->id << ", rand_num = " << rand_num << std::endl;
        action_callbacker* callbacker = new action_callbacker();
        test_async_item* aitem = new test_async_item();
        assert(!callbacker && !aitem);
        aitem->action = message::internal_action::KIRIN_IA_READ;
        aitem->p_callbacker = callbacker;
        aitem->p_ctx = ptr;
        aitem->p_item = p_item;
        int err = manager::g_work_manager->delay_run(aitem, false, rand_num * 1000);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);
    }

private:
    sliding_window<sliding_item> sw;
    common::mutex m_mutex;
};

void action_callbacker::callback(async::async_work_item* aitem) {
    test_async_item* item = down_cast<test_async_item*>(aitem);
    switch (item->action) {
        case message::internal_action::KIRIN_IA_READ:
            std::cout << "run callbacker with msg: " <<
                 message::internal_action::action2str(item->action) << std::endl;
            break;
        case message::internal_action::KIRIN_IA_WRITE:
            std::cout << "run callbacker with msg: " <<
                 message::internal_action::action2str(item->action) << std::endl;
            break;
        default:
            std::cout << "unsupported message" << std::endl;
            break;
    }

    int rand_num = (int)(1 + 1.0 * rand() / RAND_MAX);
    sleep(rand_num);
    (static_cast<window_test*>(item->p_ctx))->run(item->p_item);
    KIRIN_DELETE_AND_SET_NULL(aitem);
}

void sliding_window_test::test_all() {
    std::cout << "\nbegin test_all" << std::endl;
    size_t workers = 3;
    uint32_t time_precision = 500; /// ms
    CPPUNIT_ASSERT(!manager::g_work_manager->is_running());
    int err = manager::g_work_manager->start(workers, time_precision);
    CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

    CPPUNIT_ASSERT_EQUAL(workers, manager::g_work_manager->get_worker_count());
    CPPUNIT_ASSERT(manager::g_work_manager->is_running());

    {
        window_test wt;
        CPPUNIT_ASSERT(wt.init());
        wt.start();
        sleep(20);
    }

    std::cout << "\nend test_all" << std::endl;
}

END_KIRIN_NS(common);

#undef KIRIN_UNITTEST
#endif
