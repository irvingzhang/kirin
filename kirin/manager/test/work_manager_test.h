#ifndef KIRIN_MANAGER_WORK_MANAGER_TEST_H
#define KIRIN_MANAGER_WORK_MANAGER_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/manager/work_manager.h"

BEGIN_KIRIN_NS(manager);

class work_manager_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(work_manager_test);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST(test_2);
    CPPUNIT_TEST(test_3);
    CPPUNIT_TEST(test_4);
    CPPUNIT_TEST_SUITE_END();

public:
    work_manager_test();
    virtual ~work_manager_test();

public:
    void setUp();
    void tearDown();
    void test_1();
    void test_2();
    void test_3();
    void test_4();
};

END_KIRIN_NS(manager);
#endif
