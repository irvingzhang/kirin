#ifndef KIRIN_MESSAGE_INTERNAL_MESSAGE_H
#define KIRIN_MESSAGE_INTERNAL_MESSAGE_H

#include "kirin/message/message_base.h"
#include "kirin/message/serialize_util.h"
#include "kirin/message/deserialize_util.h"

BEGIN_KIRIN_NS(message);

struct internal_action {
    enum {
        KIRIN_IA_READ = 1,
        KIRIN_IA_WRITE,
    };

    static std::string action2str(int action) {
        switch (action) {
            case KIRIN_IA_READ:
                return "read";
            case KIRIN_IA_WRITE:
                return "write";
            default:
                return "unsupported action";
        }
    } 
};

class internal_message: public message_base {
public:
    internal_message(): message_base(false) {}
    internal_message(int action) {
        set_mid(action);
    }
    virtual ~internal_message() {}
    
    virtual std::string serialize() {
        return m_serialize ? (m_serializer.init() << m_service_group_id 
            << m_service_id << m_msg_id << m_uuid).get_buf() : "";
    }

    virtual void deserialize(const std::string& buf) {
        m_serialize ? m_deserializer.init(buf) >> m_service_group_id
            >> m_service_id >> m_msg_id >> m_uuid : m_deserializer.init(buf);
    }

private:
    serializer m_serializer;
    deserializer m_deserializer;
};

END_KIRIN_NS(message);
#endif
