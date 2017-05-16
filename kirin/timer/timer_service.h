#ifndef KIRIN_TIMER_TIMER_SERVICE_H
#define KIRIN_TIMER_TIMER_SERVICE_H

#include <map>
#include "kirin/common/lock_wrapper.h"
#include "kirin/job/base_item.h"

BEGIN_KIRIN_NS(timer);

extern uint32_t g_tick_precision;
extern uint64_t g_last_tick, g_cycle_start, g_cycle_in_tick;

class timer_service {

typedef std::multimap<uint64_t/*deadline*/, job::item_base*, std::less<uint64_t> > multi_map;
typedef std::multimap<uint64_t, job::item_base*, std::less<uint64_t> >::iterator multi_map_iterator;

public:
    timer_service();
    virtual ~timer_service();

    bool add_to_timer(uint64_t run_tick, job::item_base* p_item);
    bool remove_from_timer(job::item_base* p_item);
    virtual void run();

protected:
    virtual int do_process(job::item_base* p_item,
                           bool is_callbacker_referred,
                           bool is_emergent) = 0;

protected:
    common::mutex m_mutex;

private:
    multi_map m_timer_map;
};

END_KIRIN_NS(timer);
#endif
