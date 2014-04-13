#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Protocol
    
    Base message protocol, all other protocols are derived from this.
*/
#include "Core/Types.h"
#include "Messaging/Types.h"
#include "Messaging/Message.h"

namespace Oryol {
namespace Messaging {

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
        static const char* ToString(Messaging::MessageIdType c) {
            return "InvalidMessageId";
        };
        /// convert string to message id
        static Messaging::MessageIdType FromString(const char* str) {
            return Messaging::InvalidMessageId;
        };
    };

    /// factory class for the protocol
    class Factory {
    public:
        static Core::Ptr<Messaging::Message> Create(Messaging::MessageIdType id) {
            return Messaging::Message::Create();
        };
    };
};

} // namespace Messaging
} // namespace Oryol
