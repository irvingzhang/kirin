#ifndef KIRIN_COMMON_SLIDING_WINDOW_TEST_H
#define KIRIN_COMMON_SLIDING_WINDOW_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/common/sliding_window.h"

BEGIN_KIRIN_NS(common);

class sliding_window_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(sliding_window_test);
    CPPUNIT_TEST(test_all);
    CPPUNIT_TEST_SUITE_END();

public:
    sliding_window_test();
    virtual ~sliding_window_test();

public:
    void setUp();
    void tearDown();
    void test_all();
};

END_KIRIN_NS(common);
#endif
