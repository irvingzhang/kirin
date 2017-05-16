#ifndef KIRIN_MESSAGE_DESERIALIZE_UTIL_H
#define KIRIN_MESSAGE_DESERIALIZE_UTIL_H

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <stdexcept>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(message);

class deserializer {
public:
    deserializer();
    ~deserializer();

    deserializer& init(const std::string& str);
    deserializer& operator >> (std::string& str);

    template<typename T>
    deserializer& operator >> (T& dest);

    template<typename T>
    deserializer& operator >> (std::vector<T>& vec);

    template<typename K, typename V>
    deserializer& operator >> (std::map<K, V>& pairs);

private:
    inline deserializer& copy_value(std::string& dest);
    inline deserializer& copy_value(void* dest, const uint32_t size);

private:
    const char* m_handle_ptr;
    uint32_t m_remain_len;
};

deserializer::deserializer() {
}

deserializer::~deserializer() {
}

deserializer& deserializer::init(const std::string& str) {
    m_handle_ptr = str.data();
    m_remain_len = static_cast<uint32_t>(str.size());

    return *this;
}

deserializer& deserializer::operator >> (std::string& str) {
    return this->copy_value(str);
}

template<typename T>
deserializer& deserializer::operator >> (T& dest) {
    return this->copy_value(&dest, sizeof(dest));
}

template<typename T>
deserializer& deserializer::operator >> (std::vector<T>& vec) {
    uint32_t size = 0;
    this->copy_value(&size, sizeof(size));

    for (uint32_t index = 0; index < size; ++index) {
        T tmp;
        (*this) >> tmp;
        vec.push_back(tmp);
    }

    return *this;
}

template<typename K, typename V>
deserializer& deserializer::operator >> (std::map<K, V>& pairs) {
    uint32_t size = 0;
    this->copy_value(&size, sizeof(size));

    for (uint32_t index = 0; index < size; ++index) {
        K key;
        V val;
        (*this) >> key >> val;
        pairs.insert(std::make_pair(key, val));
    }

    return *this;
}

deserializer& deserializer::copy_value(void* dest, const uint32_t size) {
    if (m_remain_len < size) throw std::runtime_error("no enough size");

    ::memcpy(dest, m_handle_ptr, size);
    m_handle_ptr += size;
    m_remain_len -= size;

    return *this;
}

deserializer& deserializer::copy_value(std::string& dest) {
    uint32_t size = 0;
    this->copy_value(&size, sizeof(size));

    if (m_remain_len < size) throw std::runtime_error("no enough size");
    dest.assign(static_cast<const char*>(m_handle_ptr), size);
    m_handle_ptr += size;
    m_remain_len -= size;

    return *this;
}

END_KIRIN_NS(message);
#endif
