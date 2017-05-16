#include <iostream>
#include "kirin/message/test/serialize_util_test.h"

BEGIN_KIRIN_NS(message);
CPPUNIT_TEST_SUITE_REGISTRATION(serialize_util_test);

serialize_util_test::serialize_util_test() {
}

serialize_util_test::~serialize_util_test() {
}

void serialize_util_test::setUp() {
    std::cout << "serialize_util_test setup success" << std::endl;
}

void serialize_util_test::tearDown() {
    std::cout << "serialize_util_test tearDown success" << std::endl;
}

void serialize_util_test::test_all() {
    std::cout << "\nbegin test_all" << std::endl;

    serializer ser;
    deserializer desr;
    {
        std::cout << "\tstart case1: string" << std::endl;
        
        std::string test_str = "I'm a software engineer!";
        ser.init() << test_str;

        std::string res("");
        desr.init(ser.get_buf()) >> res;
        std::cout << "deserialize string: " << res << std::endl;
        CPPUNIT_ASSERT_EQUAL(test_str, res);

        std::cout << "\tend case1" << std::endl;
    }

    {
        std::cout << "\tstart case2: uint64_t" << std::endl;

        uint64_t test_int = static_cast<uint64_t>(12345947);
        ser.init() << test_int;

        uint64_t res = 0;
        desr.init(ser.get_buf()) >> res;
        std::cout << "deserialize uint64_t: " << res << std::endl;
        CPPUNIT_ASSERT_EQUAL(test_int, res);

        std::cout << "\tend case2" << std::endl;
    }

    {
        std::cout << "\tstart case3: vector<string>" << std::endl;

        std::string test_array[] = {"helen", "jack", "john"};
        size_t expect_size = sizeof(test_array) / sizeof(test_array[0]);
        std::vector<std::string> test_vec;
        test_vec.insert(test_vec.end(), test_array, test_array + expect_size);
        CPPUNIT_ASSERT_EQUAL(expect_size, test_vec.size());

        ser.init() << test_vec;

        std::vector<std::string> res;
        desr.init(ser.get_buf()) >> res;

        CPPUNIT_ASSERT_EQUAL(test_vec.size(), res.size());
        for (size_t index = 0; index < res.size(); ++index) {
            std::vector<std::string>::iterator it = std::find(res.begin(), res.end(), test_array[index]);
            CPPUNIT_ASSERT(it != res.end());
            CPPUNIT_ASSERT_EQUAL(test_array[index], *it);
            std::cout << "find vector element: " << *it << std::endl;
        }

        std::cout << "\tend case3" << std::endl;
    }

    {
        std::cout << "\tstart case4: map<string, uint32_t>" << std::endl;

        std::map<std::string, uint32_t> pairs;
        pairs["jack"] = 1938;
        pairs["john"] = 1989;
        pairs["helen"] = 1990;

        ser.init() << pairs;

        std::map<std::string, uint32_t> res;
        desr.init(ser.get_buf()) >> res;

        CPPUNIT_ASSERT_EQUAL(pairs.size(), res.size());
        for (std::map<std::string, uint32_t>::const_iterator it = res.begin();
                it != res.end(); ++it) {
            std::map<std::string, uint32_t>::const_iterator inner_it = pairs.find(it->first);
            CPPUNIT_ASSERT(inner_it != pairs.end());
            CPPUNIT_ASSERT_EQUAL(inner_it->second, it->second);
            std::cout << "key: " << it->first << ", value: " << it->second << std::endl;
        }

        std::cout << "\tend case4" << std::endl;
    }

    std::cout << "end test_all\n" << std::endl;
}

END_KIRIN_NS(message);


