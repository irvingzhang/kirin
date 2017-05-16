#include <iostream>
#include <sys/syscall.h>
#include "kirin/job/test/thread_pool_test.h"
#include "kirin/job/base_item.h"
#include "kirin/common/memory_op.h"

BEGIN_KIRIN_NS(job);
CPPUNIT_TEST_SUITE_REGISTRATION(thread_pool_test);

thread_pool_test::thread_pool_test() {
}

thread_pool_test::~thread_pool_test() {
}

void thread_pool_test::setUp() {
    std::cout << "setup success" << std::endl;
}

void thread_pool_test::tearDown() {
    std::cout << "tearDown success" << std::endl;
}

static void work1(item_base* base){
    std::cout << "run work1 in thread: " << syscall(SYS_gettid) << std::endl;
    KIRIN_DELETE_AND_SET_NULL(base);
}

static void work2(item_base* base) {
    sleep(1);
    std::cout << "run work2 after slepp in thread: " << syscall(SYS_gettid) << std::endl;
    KIRIN_DELETE_AND_SET_NULL(base);
}

void thread_pool_test::test_start() {
    std::cout << "\nbegin test_start" << std::endl;
    size_t max_workers = 3;
    {
        std::cout << "\trun case 1:" << std::endl;
        thread_pool thpool(TT_CPU, max_workers, false);
        CPPUNIT_ASSERT_EQUAL(true, thpool.start(max_workers));
        CPPUNIT_ASSERT_EQUAL(max_workers, thpool.get_worker_count());
        CPPUNIT_ASSERT_EQUAL(0, thpool.get_total_job_count());
 
        work_item* item1 = new work_item;
        item1->pfunc = work2;        
        CPPUNIT_ASSERT_EQUAL(true, thpool.append_job((item_base*)(item1), 1));

        work_item* item2 = new work_item;
        item2->pfunc = work1;
        CPPUNIT_ASSERT_EQUAL(true, thpool.append_job((item_base*)(item2), 1));

        CPPUNIT_ASSERT(thpool.get_processing_jobs(1) >= 1);
        CPPUNIT_ASSERT_EQUAL(false, thpool.cancel_job((item_base*)(item1), 1));
        CPPUNIT_ASSERT_EQUAL(true, thpool.cancel_job((item_base*)(item2), 1));
        KIRIN_DELETE_AND_SET_NULL(item2);
    
        CPPUNIT_ASSERT_EQUAL(true, thpool.stop());
        std::cout << "\tend case 1\n" << std::endl;
    }

    {
        std::cout << "\tstart case 2" << std::endl;
        thread_pool thpool(TT_CPU, max_workers + 10, false);
        CPPUNIT_ASSERT_EQUAL(true, thpool.start(max_workers));
        CPPUNIT_ASSERT_EQUAL(max_workers, thpool.get_worker_count());
        CPPUNIT_ASSERT_EQUAL(0, thpool.get_total_job_count());

        work_item* item1 = new work_item;
        item1->pfunc = work1;

        CPPUNIT_ASSERT_EQUAL(true, thpool.append_job((item_base*)(item1), 1));

        CPPUNIT_ASSERT_EQUAL(true, thpool.stop());
        std::cout << "\tend case 2" << std::endl;
    }

    std::cout << "end test_start\n" << std::endl;
}

END_KIRIN_NS(job);


