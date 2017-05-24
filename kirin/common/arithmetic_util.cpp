#include <limits>
#include <cmath>
#include "kirin/common/arithmetic_util.h"

BEGIN_KIRIN_NS(common);

bool is_equal(const double src, const double dest) {
    return fabs(src - dest) < std::numeric_limits<double>::epsilon();
}

uint16_t log_2(uint32_t nNum) {
    for (uint16_t index = 0; index < 32; ++index) {
        if (nNum == (1UL << index)) return index;
    }

    return 0;
}

END_KIRIN_NS(common);
