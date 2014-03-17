#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Message
    @brief base class for messages
*/
#include "Core/Config.h"
#include "Core/RefCounted.h"
#include "Messaging/Types.h"

namespace Oryol {
namespace Messaging {

class Message : public Core::RefCounted {
    OryolClassDecl(Message);
public:
    /// constructor
    Message();
    /// destructor
    virtual ~Message();
    
    /// test if this message belongs to a protocol
    virtual bool IsMemberOf(ProtocolIdType protId) const;
    /// get the class message id
    static MessageIdType ClassMessageId();
    /// get the object message id
    MessageIdType MessageId() const;
    /// set message to Handled state
    void SetHandled();
    /// cancel the message
    void SetCancelled();
    /// return true if the message is in Pending state
    bool Pending() const;
    /// return true if the message has been handled
    bool Handled() const;
    /// return true if the message is in cancelled state
    bool Cancelled() const;
    
    /// get the encoded size of the message
    virtual int32 EncodedSize() const;
    /// encode the message to raw memory, maxBytes must be at least EncodedSize()
    virtual uint8* Encode(uint8* dstPtr, const uint8* maxValidPtr) const;
    /// decode the message from raw memory
    virtual const uint8* Decode(const uint8* srcPtr, const uint8* maxValidPtr);

protected:
    MessageIdType msgId;
    #if ORYOL_HAS_THREADS
    std::atomic<bool> handled;
    std::atomic<bool> cancelled;
    #else
    bool handled;
    bool cancelled;
    #endif
};

//------------------------------------------------------------------------------
inline Message::Message() :
msgId(InvalidMessageId),
handled(false),
cancelled(false) {
    // empty
}

//------------------------------------------------------------------------------
inline MessageIdType
Message::MessageId() const {
    return this->msgId;
}

} // namespace Messaging
} // namespace Oryol