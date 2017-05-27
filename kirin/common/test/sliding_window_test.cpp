#ifndef KIRIN_UNITTEST
#define KIRIN_UNITTEST

#include <iostream>
#include <sys/syscall.h>
#include "kirin/common/test/sliding_window_test.h"

BEGIN_KIRIN_NS(common);
CPPUNIT_TEST_SUITE_REGISTRATION(sliding_window_test);

sliding_window_test::sliding_window_test() {
}

sliding_window_test::~sliding_window_test() {
}

void sliding_window_test::setUp() {
    std::cout << "setup success" << std::endl;
}

void sliding_window_test::tearDown() {
    std::cout << "tearDown success" << std::endl;
}

struct sliding_item {
    int id;
};

class window_test {
public:
    bool init() {
        return sw.init(10, on_init, on_empty, on_ready);
    }

    void start() {
        sw.start(this);
    }

private:
    static void on_init(sliding_item* p_item, void* ptr) {
        memset(p_item, 0, sizeof(sliding_item));
        std::cout << "successfully on_init" << std::endl;
    }

    static void on_empty(sliding_item* p_item, void* ptr) {
        window_test* p_ctx = static_cast<window_test*>(ptr);
        static int counter = 0;
        std::cout << "on_empty" << std::endl;
        p_item->id = ++counter;
        if (counter <= 30)
            p_ctx->sw.add_item(p_item);
    }

    static void on_ready(sliding_item* p_item, void* ptr) {
        window_test* p_ctx = static_cast<window_test*>(ptr);
        std::cout << "on_ready, counter " << p_item->id << std::endl;
        p_ctx->sw.free_item(p_item);
    }

private:
    sliding_window<sliding_item> sw;
    sliding_item si;
};

void sliding_window_test::test_all() {
    std::cout << "\nbegin test_all" << std::endl;

    {
        window_test wt;
        CPPUNIT_ASSERT(wt.init());
        wt.start();
    }

    std::cout << "\nend test_all" << std::endl;
}

END_KIRIN_NS(common);

#undef KIRIN_UNITTEST
#endif
