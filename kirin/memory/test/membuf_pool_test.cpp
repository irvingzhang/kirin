#include <iostream>
#include "kirin/manager/work_manager.h"
#include "kirin/memory/object_pool_manager.h"
#include "kirin/memory/test/membuf_pool_test.h"

BEGIN_KIRIN_NS(memory);
CPPUNIT_TEST_SUITE_REGISTRATION(membuf_pool_test);

membuf_pool_test::membuf_pool_test() {
}

membuf_pool_test::~membuf_pool_test() {
}

void membuf_pool_test::setUp() {
    std::cout << "membuf_pool_test setup success" << std::endl;
}

void membuf_pool_test::tearDown() {
    std::cout << "membuf_pool_test tearDown success" << std::endl;
}

void membuf_pool_test::test_all() {
    std::cout << "\nbegin test_all" << std::endl;
    size_t workers = 3;
    uint32_t time_precision = 500; /// ms
    CPPUNIT_ASSERT(!manager::g_work_manager->is_running());
    int err = manager::g_work_manager->start(workers, time_precision);
    CPPUNIT_ASSERT_EQUAL((int)common::kirin_error::KIRIN_ERR_OK, err);

    {
        std::cout << "\tstart case 1:" << std::endl;

        membuf_pool pool("test membuf");
        mem_arg* arg = new (std::nothrow) mem_arg(512, 10, 1, 10, 0.6, 0.9);
        CPPUNIT_ASSERT(arg != NULL);

        CPPUNIT_ASSERT(pool.init(arg));
        std::list<char*> buf_list;
        for (size_t index = 0; index < 18; ++index) {
            char* p_buf = pool.alloc(0);
            CPPUNIT_ASSERT(p_buf != NULL);
            buf_list.push_back(p_buf);
        }

        std::cout << "alloc memory success" << std::endl;

        for (std::list<char*>::iterator it = buf_list.begin();
                it != buf_list.end(); ++it) {
            try {
                CPPUNIT_ASSERT(pool.free(*it));
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }

        std::cout << "free memory success" << std::endl;
        
        sleep(GC_INTERVAL/1000 + 1);
        
        std::cout << "\tend case 1" << std::endl;
    }

    std::cout << "\nend test_all" << std::endl;
}

END_KIRIN_NS(memory);
