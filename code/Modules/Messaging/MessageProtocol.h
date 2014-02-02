#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::MessageProtocol
    
    Base message protocol, all other protocols are derived from this.
*/
#include "Core/Types.h"
#include "Messaging/Message.h"

namespace Oryol {
namespace MessageProtocol {

class MessageId {
public:
    enum {
        NumMessageIds = 0,
    };
    
    /// convert message id to string
    static const char* ToString(Messaging::MessageIdType c) {
        return "InvalidMessageId";
    };
    /// convert string to message id
    static Messaging::MessageIdType FromString(const char* str) {
        return Messaging::InvalidMessageId;
    };
};

} // namespace MessageProtocol
} // namespace Oryol