#ifndef KIRIN_MESSAGE_TEST_SERIALIZER_UTIL_TEST_H
#define KIRIN_MESSAGE_TEST_SERIALIZER_UTIL_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "kirin/message/serialize_util.h"
#include "kirin/message/deserialize_util.h"

BEGIN_KIRIN_NS(message);

class serialize_util_test: public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(serialize_util_test);
    CPPUNIT_TEST(test_all);
    CPPUNIT_TEST_SUITE_END();

public:
    serialize_util_test();
    virtual ~serialize_util_test();

public:
    void setUp();
    void tearDown();
    void test_all();
};

END_KIRIN_NS(message);
#endif
