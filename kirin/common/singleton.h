#ifndef KIRIN_COMMON_SINGLETON_H
#define KIRIN_COMMON_SINGLETON_H

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "kirin/common/non_copyable.h"

BEGIN_KIRIN_NS(common);

template<class T>
class singleton: public non_copyable {
public:
    static T& instance() {
        pthread_once(&m_ponce, &singleton::init);
        return *m_instance;
    }

    static T* instance_ptr() {
        pthread_once(&m_ponce, &singleton::init);
        return m_instance;
    }

private:
    static void init() {
        m_instance = new (std::nothrow) T();
        assert(m_instance != NULL);
        atexit(destroy);
    }

    static void destroy() {
        KIRIN_DELETE_AND_SET_NULL(m_instance);
    }

private:
    static T* volatile m_instance;
    static pthread_once_t m_ponce;
};

template<class T>
T* volatile singleton<T>::m_instance = NULL;

template<class T>
pthread_once_t singleton<T>::m_ponce = PTHREAD_ONCE_INIT;

END_KIRIN_NS(common);
#endif
