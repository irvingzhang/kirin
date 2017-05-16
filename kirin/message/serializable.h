#ifndef KIRIN_MESSAGE_SERIALIZABLE_H
#define KIRIN_MESSAGE_SERIALIZABLE_H

#include "kirin/common/common.h"

BEGIN_KIRIN_NS(message);

class serializable {
public:
    virtual ~serializable() {}

    virtual std::string serialize() = 0;
    virtual void deserialize(const std::string& buf) = 0;
};
END_KIRIN_NS(message);
#endif
