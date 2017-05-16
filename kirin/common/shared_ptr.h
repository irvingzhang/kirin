#ifndef KIRIN_COMMON_SHARED_PTR_H
#define KIRIN_COMMON_SHARED_PTR_H

#include <assert.h>
#include "kirin/common/ref_counter.h"
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(job);

template<typename T>
class shared_ptr {
public:
    shared_ptr();
    explicit shared_ptr(const T* ptr);
    explicit shared_ptr(const shared_ptr<T>& pobj);
    virtual ~shared_ptr();

    T& operator*();
    T* operator->();
    bool operator==(const shared_ptr<T>& object);
    bool operator==(const T* _ptr);
    shared_ptr<T>& operator=(const shared_ptr<T>& object);
    void reset();

    size_t ref_count() {
        return m_ref_counter != NULL ? m_ref_counter->get_ref() : 0;
    }

    T* get() const {
        return m_actual_ptr;
    }

    bool unique() {
        return ref_count() == 1;
    }

    operator bool() const {
        return m_actual_ptr != NULL;
    }

    ref_counter<size_t>* get_ref_conter() const {
        return m_ref_counter;
    }

private:
    T* m_actual_ptr;
    ref_counter<size_t>* m_ref_counter;
};

template<typename T>
shared_ptr<T>::shared_ptr():
    m_actual_ptr(NULL), m_ref_counter(NULL) {}

template<typename T>
shared_ptr<T>::shared_ptr(const T* ptr):
    m_actual_ptr(ptr), m_ref_counter(NULL) {
    if (m_actual_ptr != NULL) {
        m_ref_counter = new (std::nothrow) ref_counter<size_t>();
        assert(m_ref_counter != NULL);
        m_ref_counter->add_ref();
    }
}

template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr<T>& object):
    m_actual_ptr(object.get()), m_ref_counter(object.get_ref_counter()) {
    if (m_actual_ptr != NULL) {
        assert(m_ref_counter != NULL);
        m_ref_counter->add_ref();
    }
}

template<typename T>
shared_ptr<T>::~shared_ptr() {
    reset();
}

template<typename T>
void shared_ptr<T>::reset() {
    if (m_ref_counter->get_ref() == 0) 
        KIRIN_DELETE_AND_SET_NULL(m_actual_ptr);
}

template<typename T>
T& shared_ptr<T>::operator *() {
    assert(m_actual_ptr != NULL);
    return *m_actual_ptr;
}

template<typename T>
T* shared_ptr<T>::operator ->() {
    assert(m_actual_ptr != NULL);
    return m_actual_ptr;
}

template<typename T>
bool shared_ptr<T>::operator ==(const shared_ptr<T>& object) {
    return m_actual_ptr == object.get();
}

template<typename T>
bool shared_ptr<T>::operator ==(const T* ptr) {
    return m_actual_ptr == ptr;
}

template<typename T>
shared_ptr<T>& shared_ptr<T>::operator =(const shared_ptr<T>& object) {
    if (&object == this) return *this;

    reset();
    m_actual_ptr = object.get();
    m_ref_counter = object.get_ref_counter();

    if (m_actual_ptr != NULL) {
        assert(m_ref_counter != NULL);
        m_ref_counter->add_ref();
    }

    return *this;
}

END_KIRIN_NS(job);
#endif
