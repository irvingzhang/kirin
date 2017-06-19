#ifndef KIRIN_TIMER_TIMER_H
#define KIRIN_TIMER_TIMER_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "kirin/common/string_util.h"

BEGIN_KIRIN_NS(timer);

static inline uint64_t rdtsc()  {  /// static function can only be used in current file
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)lo)|(((uint64_t)hi)<<32 );
}

uint64_t estimate_cpu_frequency(); 

uint64_t get_cpu_frequency(); 

END_KIRIN_NS(timer);

using kirin::timer::rdtsc;
using kirin::timer::get_cpu_frequency;
#endif
