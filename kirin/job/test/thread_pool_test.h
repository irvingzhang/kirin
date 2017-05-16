#ifndef KIRIN_JOB_TEST_THREAD_POOL_TEST_H
#define KIRIN_JOB_TEST_THREAD_POOL_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/job/thread_pool.h"

BEGIN_KIRIN_NS(job);

class thread_pool_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(thread_pool_test);
    CPPUNIT_TEST(test_start);
    CPPUNIT_TEST_SUITE_END();

public:
    thread_pool_test();
    virtual ~thread_pool_test();

public:
    void setUp();
    void tearDown();
    void test_start();
};

END_KIRIN_NS(job);
#endif
