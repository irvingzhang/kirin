#ifndef KIRIN_COMMON_ARITHMETIC_UTIL_H
#define KIRIN_COMMON_ARITHMETIC_UTIL_H

#include <stdint.h>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);

bool is_equal(const double src, const double dest);
uint16_t log_2(uint32_t nNum);

END_KIRIN_NS(common);
#endif
