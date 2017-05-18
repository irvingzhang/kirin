#include <iostream>
#include "kirin/timer/timer.h"
#include "kirin/async/async.h"
#include "kirin/timer/test/timer_service_test.h"

BEGIN_KIRIN_NS(timer);
CPPUNIT_TEST_SUITE_REGISTRATION(timer_service_test);

timer_service_test::timer_service_test() {
    g_tick_precision = 500; /// 500ms
    g_cycle_start = rdtsc();
    g_cycle_in_tick = get_cpu_frequency() * timer::g_tick_precision / 1000;
    g_last_tick = (rdtsc() - g_cycle_start) / g_cycle_in_tick;
}

timer_service_test::~timer_service_test() {
}

void timer_service_test::setUp() {
    std::cout << "work_manager_test setup success" << std::endl;
}

void timer_service_test::tearDown() {
    std::cout << "work_manager_test tearDown success" << std::endl;
}

class obj_with_timer: public timer_service {
public:
    virtual ~obj_with_timer() {}

protected:
    int do_process(job::item_base* p_item,
                           bool is_callbacker_referred,
                           bool is_emergent) {
        std::cout << "success fully run do_process with time tick: " 
                << g_last_tick + 1 << std::endl;
        return 0;
    }
    
public:
    size_t size() {
        return m_timer_map.size();
    }
};

void timer_service_test::test_all() {
    std::cout << "\nbegin test_all" << std::endl;

    {
        std::cout << "\tstart case 1: " << std::endl;

        obj_with_timer owt;
        owt.start();
        for (uint64_t index = 0; index < 10; ++index) {
            job::item_base* ptmp = new job::work_item();
            bool add_succ = owt.add_to_timer(0, ptmp);
            CPPUNIT_ASSERT(add_succ);
            KIRIN_DELETE_AND_SET_NULL(ptmp);
        }
        CPPUNIT_ASSERT_EQUAL((size_t)10, owt.size());        
        sleep(2);
    
        std::cout << "\tendl case 1\n" << std::endl;
    }

    {
        obj_with_timer owt;
        owt.start();
        std::cout << "\tstart case 2: " << std::endl;
        for (uint64_t index = 0; index < 10; ++index) {
            job::item_base* ptmp = new job::work_item();
            bool add_succ = owt.add_to_timer(index + g_last_tick, ptmp);
            CPPUNIT_ASSERT(add_succ);
            KIRIN_DELETE_AND_SET_NULL(ptmp);
        }
        CPPUNIT_ASSERT_EQUAL((size_t)10, owt.size());
        sleep(5);

        std::cout << "\tendl case 2\n" << std::endl;
    }

    {
        obj_with_timer owt;
        owt.start();
        std::cout << "\tstart case 3: " << std::endl;
        job::item_base* to_remove = NULL;
        for (uint64_t index = 0; index < 10; ++index) {
            async::async_work_item* ptmp = new async::async_work_item();
            uint64_t run_tick = index + g_last_tick;
            ptmp->n_run_tick = run_tick;

            if (index == 5) to_remove = ptmp;

            bool add_succ = owt.add_to_timer(run_tick, ptmp);
            CPPUNIT_ASSERT(add_succ);

            if (index != 5) {
                KIRIN_DELETE_AND_SET_NULL(ptmp);
            } else {
                to_remove = ptmp;
            }
        }
        CPPUNIT_ASSERT_EQUAL((size_t)10, owt.size());

        bool is_succ = owt.remove_from_timer(to_remove);
        CPPUNIT_ASSERT(is_succ);

        CPPUNIT_ASSERT_EQUAL((size_t)9, owt.size());
        sleep(5);
        KIRIN_DELETE_AND_SET_NULL(to_remove);
        std::cout << "\tend case 3" << std::endl;
    }
 
    std::cout << "end test_all\n" << std::endl;
}

END_KIRIN_NS(timer);

