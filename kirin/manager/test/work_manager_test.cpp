#include <iostream>
#include "kirin/message/internal_message.h"
#include "kirin/async/async.h"
#include "kirin/manager/test/work_manager_test.h"
#include "kirin/common/cast.h"
#include "kirin/common/error_def.h"
#include "kirin/common/singleton.h"

BEGIN_KIRIN_NS(manager);
CPPUNIT_TEST_SUITE_REGISTRATION(work_manager_test);

work_manager_test::work_manager_test() {
}

work_manager_test::~work_manager_test() {
}

void work_manager_test::setUp() {
    std::cout << "work_manager_test setup success" << std::endl;
}

void work_manager_test::tearDown() {
    std::cout << "work_manager_test tearDown success" << std::endl;
}

class test_callbacker: public async::callbacker {
public:
    void callback(async::async_work_item* item) {
        message::message_base* msg = item->message;
        message::internal_message* internal_msg = down_cast<message::internal_message*>(msg);
    
        uint32_t msg_id = internal_msg->get_mid();
        switch (msg_id) {
            case message::internal_action::KIRIN_IA_READ:
                std::cout << "run callbacker with msg: " <<
                     message::internal_action::action2str(msg_id) << std::endl;
                break;
            case message::internal_action::KIRIN_IA_WRITE:
                std::cout << "run callbacker with msg: " << 
                     message::internal_action::action2str(msg_id) << std::endl;
                break;
            default:
                std::cout << "unsupported message" << std::endl;    
                break;
        }
        KIRIN_DELETE_AND_SET_NULL(msg);
    }
};

class action_callbacker: public async::callbacker {
public:
    void callback(async::async_work_item* item) {
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
    }
};


void work_manager_test::test_1() {
    size_t workers = 3;
    uint32_t time_precision = 500; /// ms
        std::cout << "\tstart case 1:" << std::endl;

        CPPUNIT_ASSERT(!g_work_manager->is_running());
        int err = g_work_manager->start(workers, time_precision);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

        CPPUNIT_ASSERT_EQUAL(workers, g_work_manager->get_worker_count());
        CPPUNIT_ASSERT(g_work_manager->is_running());

        test_callbacker* callbacker = new (std::nothrow) test_callbacker;
        message::message_base *msg = new (std::nothrow) 
                message::internal_message(message::internal_action::KIRIN_IA_WRITE);
        assert(callbacker != NULL && msg != NULL);
    
        async::async_work_item* async_item = new (std::nothrow) async::async_work_item;
        assert(async_item != NULL);
        async_item->message = msg;
        async_item->p_callbacker = callbacker;

        err = g_work_manager->append_job(async_item, false, false);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);
        KIRIN_DELETE_AND_SET_NULL(async_item);
        sleep(1);
        g_work_manager->stop(true);   
        std::cout << "\tend case 1\n" << std::endl;
}

void work_manager_test::test_2() {
    size_t workers = 3;
    uint32_t time_precision = 500; /// ms
        std::cout << "\ttest case 2: " << std::endl;
     
        CPPUNIT_ASSERT(!g_work_manager->is_running());
        int err = g_work_manager->start(workers, time_precision);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

        CPPUNIT_ASSERT_EQUAL(workers, g_work_manager->get_worker_count());
        CPPUNIT_ASSERT(g_work_manager->is_running());

        test_callbacker* callbacker = new (std::nothrow) test_callbacker;
        message::message_base* msg = new (std::nothrow) 
                 message::internal_message(message::internal_action::KIRIN_IA_READ);
        assert(callbacker != NULL && msg != NULL);

        async::async_work_item* async_item = new (std::nothrow) async::async_work_item;
        assert(async_item != NULL);
        async_item->message = msg;
        async_item->p_callbacker = callbacker;
 
        err = g_work_manager->delay_run(async_item, false, 1000);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);
        sleep(2);
        KIRIN_DELETE_AND_SET_NULL(async_item);
        g_work_manager->stop(true);
        std::cout << "\tend case 2\n" << std::endl;
    }

void work_manager_test::test_3() {
    size_t workers = 3;
    uint32_t time_precision = 500; /// ms
        std::cout << "\ttest case 3: " << std::endl;

        CPPUNIT_ASSERT(!g_work_manager->is_running());
        int err = g_work_manager->start(workers, time_precision);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);
                 
        CPPUNIT_ASSERT_EQUAL(workers, g_work_manager->get_worker_count());
        CPPUNIT_ASSERT(g_work_manager->is_running());

        test_callbacker* callbacker = new (std::nothrow) test_callbacker;
        async::async_work_item* async_item = new (std::nothrow) async::async_work_item;
        message::message_base* msg = new (std::nothrow) 
                message::internal_message(message::internal_action::KIRIN_IA_WRITE);
        assert(async_item != NULL && callbacker != NULL && msg != NULL);
        async_item->message = msg;
        async_item->p_callbacker = callbacker;

        err = g_work_manager->delay_run(async_item, false, 1000);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

        err = g_work_manager->cancel_run(async_item);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

        sleep(2);
        KIRIN_DELETE_AND_SET_NULL(async_item);
        KIRIN_DELETE_AND_SET_NULL(msg);
        g_work_manager->stop(true);

        std::cout << "\tend case 3\n" << std::endl;
    }

void work_manager_test::test_4() {
    size_t workers = 3;
    uint32_t time_precision = 500; /// ms
        std::cout << "\ttest case 4: " << std::endl;

        CPPUNIT_ASSERT(!g_work_manager->is_running());
        int err = g_work_manager->start(workers, time_precision);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

        CPPUNIT_ASSERT_EQUAL(workers, g_work_manager->get_worker_count());
        CPPUNIT_ASSERT(g_work_manager->is_running());

        action_callbacker* callbacker = new (std::nothrow) action_callbacker;
        assert(callbacker != NULL);

        async::async_work_item* async_item = new async::async_work_item;
        async_item->action = message::internal_action::KIRIN_IA_READ;
        async_item->p_callbacker = callbacker;

        err = g_work_manager->delay_run(async_item, false, 1000);
        CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);
        sleep(2);
        KIRIN_DELETE_AND_SET_NULL(async_item);
        g_work_manager->stop(true);
        std::cout << "\tend case 4\n" << std::endl;
    }

END_KIRIN_NS(manager);

