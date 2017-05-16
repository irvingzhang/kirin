#ifndef KIRIN_COMMON_ERROR_DEF_H
#define KIRIN_COMMON_ERROR_DEF_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);

struct kirin_error {
    enum {
        KIRIN_ERR_OK = 0,
        KIRIN_ERR_SYS = -1,
        KIRIN_ERR_INVALID_ARGUMENTS = -2,
        KIRIN_ERR_NO_THREAD_WORKER = -3,
        KIRIN_ERR_ALREADY_EXECUTE = -4,
    };

    static std::string err2str(int err) {
        switch (err) {
            case kirin_error::KIRIN_ERR_OK:
                return "ok";
            case kirin_error::KIRIN_ERR_SYS:
                return "system error";
            case kirin_error::KIRIN_ERR_INVALID_ARGUMENTS:
                return "invalid arguments";
            case kirin_error::KIRIN_ERR_NO_THREAD_WORKER:
                return "no wokring thread";
            case kirin_error::KIRIN_ERR_ALREADY_EXECUTE:
                return "already execute";
            default:
                return "error not defined";
        }
    }
};

END_KIRIN_NS(common);
#endif
