#ifndef KIRIN_MANAGER_WORK_MANAGER_TEST_H
#define KIRIN_MANAGER_WORK_MANAGER_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/manager/work_manager.h"

BEGIN_KIRIN_NS(manager);

class work_manager_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(work_manager_test);
    CPPUNIT_TEST(test_all);
    CPPUNIT_TEST_SUITE_END();

public:
    work_manager_test();
    virtual ~work_manager_test();

public:
    void setUp();
    void tearDown();
    void test_all();
};

END_KIRIN_NS(manager);
#endif
