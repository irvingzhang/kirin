#ifndef KIRIN_COMMON_NONCOPYABLE_H
#define KIRIN_COMMON_NONCOPYABLE_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);

class non_copyable {
protected:
    non_copyable() {}
    virtual ~non_copyable() {}

private:
    non_copyable(const non_copyable&);
    const non_copyable& operator=(const non_copyable&);
};

END_KIRIN_NS(common);
#endif
