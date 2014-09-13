#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Protocol
    @ingroup Messaging
    @brief message protocol base class
*/
#include "Core/Types.h"
#include "Messaging/Types.h"
#include "Messaging/Message.h"

namespace Oryol {

class Protocol {
public:
    /// get the protocol id
    static ProtocolIdType GetProtocolId() {
        return 'BASE';
    };

    /// protocol message ids
    class MessageId {
    public:
        enum {
            NumMessageIds = 0,
        };
        
        /// convert message id to string
        static const char* ToString(MessageIdType c) {
            return "InvalidMessageId";
        };
        /// convert string to message id
        static MessageIdType FromString(const char* str) {
            return InvalidMessageId;
        };
    };

    /// factory class for the protocol
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id) {
            return Message::Create();
        };
    };
};

} // namespace Oryol
