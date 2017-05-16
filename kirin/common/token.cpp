#include "kirin/common/atomic_op.h"
#include "kirin/common/token.h"

BEGIN_KIRIN_NS(common);
token::token() {
    this->init();
}

token::~token() {
}

void token::init(int tid) {
    m_token = tid;
}

bool token::check(int expected) {
    return expected == ATOMIC_GET(&m_token);
}

int token::release(int tid) {
    return atomic_cmp_swap(&m_token, 0, tid);
}

bool token::try_acquire(int tid) {
    return atomic_cmp_swap(&m_token, tid, 0);
}

END_KIRIN_NS(common);
