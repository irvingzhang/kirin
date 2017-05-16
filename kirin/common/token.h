#ifndef KIRIN_COMMON_TOKEN_H
#define KIRIN_COMMON_TOKEN_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);
class token {
public:
    token();
    ~token();

    void init(int tid = 0);
    bool check(int expected);
    int release(int tid);
    bool try_acquire(int tid);

private:
    volatile int m_token;
};

END_KIRIN_NS(common);
#endif
