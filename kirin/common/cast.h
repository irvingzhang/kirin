#ifndef KIRIN_COMMON_CAST_H
#define KIRIN_COMMON_CAST_H

BEGIN_KIRIN_NS(common);

// a safe version of static_cast or const_cast
template<typename To, typename From>
inline To implicit_cast(From const& f) {
    return f;
}

template<typename To, typename From>
inline To down_cast(From* f) {
    if (false) implicit_cast<From*, To>(0); // check in compile process
    
    return static_cast<To>(f);
}
END_KIRIN_NS(common);

using kirin::common::implicit_cast;
using kirin::common::down_cast;

#endif
