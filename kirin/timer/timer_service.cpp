#include <iostream>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include "kirin/timer/timer_service.h"
#include "kirin/timer/timer.h"
#include "kirin/common/token.h"
#include "kirin/common/lock.h"
#include "kirin/common/cast.h"
#include "kirin/async/async.h"

BEGIN_KIRIN_NS(timer);

uint32_t g_tick_precision = 0;
uint64_t g_last_tick = 0, g_cycle_start = 0, g_cycle_in_tick = 1;

timer_service::timer_service(): m_running(false),
                                m_efd(-1) {
    m_timer_map.clear();
}

void timer_service::start() {
    m_running = true;
    m_efd = ::epoll_create(16);

    struct lambda {
        static void run(void* p_service) {
            std::cout << "timer_service thread started: " << syscall(SYS_gettid) << std::endl;
            static_cast<timer_service*>(p_service)->run_timer();
        }
    };

    m_thread.create(new job::task(&lambda::run, this));
}

timer_service::~timer_service() {
    m_running = false;

    if (m_efd != -1) ::close(m_efd);
    m_thread.join();
    m_timer_map.clear();
}

void timer_service::run_timer() {
    struct epoll_event ev_set[64];

    while (true) {
        ::epoll_wait(m_efd, ev_set, 64, g_tick_precision);
    
        if (!m_running) break;

        this->run();
    }
}

bool timer_service::is_running() {
    return m_running;
}

void timer_service::run() {
    static common::token tk;

    if (!tk.try_acquire(1)) return;

    const uint64_t curr_tick = (timer::rdtsc() - g_cycle_start) / g_cycle_in_tick;
    if (curr_tick == g_last_tick) {
        tk.release(1);
        return;
    }

    g_last_tick = curr_tick;
    while(true) { 
        common::exclusive_lock lock(m_mutex);
        multi_map_iterator it = m_timer_map.begin();
        if (it == m_timer_map.end() || curr_tick < it->first) {
            lock.unlock();
            break;
        }

        std::pair<multi_map_iterator, multi_map_iterator> inner_it = m_timer_map.equal_range(it->first);
        while (inner_it.first != inner_it.second) {
            do_process(inner_it.first->second, true, true);
            ++inner_it.first;
        }

        m_timer_map.erase(it->first);
        lock.unlock();
    }

    tk.release(1);    
}

bool timer_service::add_to_timer(uint64_t run_tick, job::item_base* p_item) {
    common::exclusive_lock lock(m_mutex);
    multi_map_iterator it =m_timer_map.insert(std::pair<uint64_t, job::item_base*>(run_tick, p_item));

    lock.unlock();
    return it != m_timer_map.end();
}

bool timer_service::remove_from_timer(job::item_base* p_item) {
    if (p_item != NULL && is_instance_of<async::async_work_item>(p_item)) {
        async::async_work_item* p_actual = down_cast<async::async_work_item*>(p_item);
        common::exclusive_lock lock(m_mutex);
        std::pair<multi_map_iterator, multi_map_iterator>  it = m_timer_map.equal_range(p_actual->n_run_tick);

        while (it.first != it.second) {
            if (it.first->second == p_item) {
                m_timer_map.erase(it.first);
                lock.unlock();
                return true;
            }
            ++it.first;
        }
        lock.unlock();
    }

    return false;
}

END_KIRIN_NS(timer);
