#ifndef KIRIN_MEMORY_MEMBUF_POOL_TEST_H
#define KIRIN_MEMORY_MEMBUF_POOL_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/memory/membuf_pool.h"

BEGIN_KIRIN_NS(memory);

class membuf_pool_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(membuf_pool_test);
    CPPUNIT_TEST(test_all);
    CPPUNIT_TEST_SUITE_END();

public:
    membuf_pool_test();
    virtual ~membuf_pool_test();

public:
    void setUp();
    void tearDown();
    void test_all();
};

END_KIRIN_NS(memory);
#endif
