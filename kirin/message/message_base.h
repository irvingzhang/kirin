#ifndef KIRIN_MESSAGE_MESSAGE_BASE_H
#define KIRIN_MESSAGE_MESSAGE_BASE_H

#include "kirin/common/common.h"
#include "kirin/message/serializable.h"

BEGIN_KIRIN_NS(message);

class message_base: public serializable {
public:
    message_base():
            m_service_group_id(0),
            m_service_id(0),
            m_msg_id(0),
            m_uuid(""),
            m_serialize(false) {
    }

    explicit message_base(bool need_serialize):
            m_service_group_id(0),
            m_service_id(0),
            m_msg_id(0),
            m_uuid(""),
            m_serialize(need_serialize) {
    }

    message_base(uint32_t msg_id, bool need_serialize):
            m_service_group_id(0),
            m_service_id(0),
            m_msg_id(msg_id),
            m_uuid(""),
            m_serialize(need_serialize) {
    }

    virtual ~message_base() {
    }

    uint32_t get_sgid() const {
        return m_service_group_id;
    }

    uint32_t get_sid() const {
        return m_service_id;
    }

    uint32_t get_mid() const {
        return m_msg_id;
    }

    std::string get_uuid() const {
        return m_uuid;
    }

    void set_sgid(uint32_t service_group_id) {
        m_service_group_id = service_group_id;
    }

    void set_sid(uint32_t service_id) {
        m_service_id = service_id;
    }

    void set_mid(uint32_t msg_id) {
        m_msg_id = msg_id;
    }

    void set_uuid(std::string uuid) {
        m_uuid = uuid;
    }

protected:
    uint32_t m_service_group_id;
    uint32_t m_service_id;
    uint32_t m_msg_id;
    std::string m_uuid;
    bool m_serialize; /// encode or not
};

END_KIRIN_NS(message);
#endif
