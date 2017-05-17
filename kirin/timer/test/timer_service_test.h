#ifndef KIRIN_TIMER_TIMER_SERVICE_TEST_H
#define KIRIN_TIMER_TIMER_SERVICE_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/timer/timer_service.h"

BEGIN_KIRIN_NS(timer);

class timer_service_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(timer_service_test);
    CPPUNIT_TEST(test_all);
    CPPUNIT_TEST_SUITE_END();

public:
    timer_service_test();
    virtual ~timer_service_test();

public:
    void setUp();
    void tearDown();
    void test_all();
};

END_KIRIN_NS(timer);
#endif
