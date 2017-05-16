#include "kirin/timer/timer_service.h"
#include "kirin/timer/timer.h"
#include "kirin/common/token.h"
#include "kirin/common/lock.h"
#include "kirin/common/cast.h"
#include "kirin/async/async.h"

BEGIN_KIRIN_NS(timer);

uint32_t g_tick_precision = 0;
uint64_t g_last_tick = 0, g_cycle_start = 0, g_cycle_in_tick = 0;

timer_service::timer_service() {
    m_timer_map.clear();
}

timer_service::~timer_service() {
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

        m_timer_map.erase(it);
        lock.unlock();
    }

    tk.release(1);    
}

bool timer_service::add_to_timer(uint64_t run_tick, job::item_base* p_item) {
    common::exclusive_lock lock(m_mutex);
    m_timer_map.insert(std::make_pair(run_tick, p_item));

    return true;
}

bool timer_service::remove_from_timer(job::item_base* p_item) {
    if (is_instance_of<async::async_work_item>(p_item)) {
        async::async_work_item* p_actual = down_cast<async::async_work_item*>(p_item);
        std::pair<multi_map_iterator, multi_map_iterator>  it = m_timer_map.equal_range(p_actual->n_run_tick);

        while (it.first != it.second) {
            if (it.first->second == p_item) {
                m_timer_map.erase(it.first);
                return true;
            }
            ++it.first;
        }
    }

    return false;
}

END_KIRIN_NS(timer);
