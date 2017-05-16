#ifndef KIRIN_MESSAGE_SERIALIZE_UTIL_H
#define KIRIN_MESSAGE_SERIALIZE_UTIL_H

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include "kirin/common/common.h"

BEGIN_KIRIN_NS(message);
class serializer {
public:
    serializer();
    ~serializer();

    serializer& init() {
        m_buf.clear();
        return *this;
    }

    const std::string& get_buf() const {
        return m_buf;
    }

    serializer& operator << (const std::string& str);

    template<typename T>
    serializer& operator << (const T& arg);

    template<typename T>
    serializer& operator << (const std::vector<T>& vec);

    template<typename K, typename V>
    serializer& operator << (const std::map<K, V>& pairs);

private:
    inline serializer& copy_value(const std::string& str);
    inline serializer& copy_value(const void* src, size_t size);

private:
    std::string m_buf;
};

serializer::serializer(): m_buf("") {
}

serializer::~serializer() {
}

serializer& serializer::operator << (const std::string& str) {
    return this->copy_value(str);
}

template<typename T>
serializer& serializer::operator << (const T& arg) {
    return this->copy_value(&arg, sizeof(arg));
}

template<typename T>
serializer& serializer::operator << (const std::vector<T>& vec) {
    uint32_t size = static_cast<uint32_t>(vec.size());
    this->copy_value(&size, sizeof(size));

    for (typename std::vector<T>::const_iterator it = vec.begin();
            it != vec.end(); ++it) {
        (*this) << (*it);
    }

    return *this;
}

template<typename K, typename V>
serializer& serializer::operator << (const std::map<K, V>& pairs) {
    uint32_t size = static_cast<uint32_t>(pairs.size());
    this->copy_value(&size, sizeof(size));

    for (typename std::map<K, V>::const_iterator it = pairs.begin();
            it != pairs.end(); ++it) {
        (*this) << it->first << it->second;
    }

    return *this;
}

serializer& serializer::copy_value(const std::string& str) {
    const uint32_t size = static_cast<uint32_t>(str.size());
    this->copy_value(&size, sizeof(size));
    this->copy_value(str.data(), size);
    return *this;
}

serializer& serializer::copy_value(const void* src, size_t size) {
    m_buf.append(static_cast<const char*>(src), size);
    return *this;
}

END_KIRIN_NS(message);
#endif
