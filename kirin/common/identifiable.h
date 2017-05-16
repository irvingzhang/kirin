#ifndef KIRIN_COMMON_IDENTIFIABLE_H
#define KIRIN_COMMON_IDENTIFIABLE_H

#include <typeinfo>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(common);

class identifiable {
public:
    virtual ~identifiable() {}
    virtual const identifiable* get_class() const = 0;
};

template<typename T>
inline bool is_instance_of(identifiable *const pobj) {
    return typeid(*(pobj->get_class())) == typeid(T);
}

END_KIRIN_NS(common);

using kirin::common::is_instance_of;
#endif
