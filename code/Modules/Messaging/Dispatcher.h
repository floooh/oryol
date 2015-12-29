#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Dispatcher
    @ingroup Messaging
    @brief call message handler functions on incoming messages
    
    A Dispatcher is a Port where message handler functions for a specific
    message protocol can subscribe to. One specific Dispatcher object
    can only handle messages from one specific protocol. The Protocol must
    be given as a template argument.
 
    When a Message arrives in the Put method, the subscribed handler function
    is looked up in a jump table and called. If no handler function exists
    for the message, nothing will happen.
 
    The message handler function is expected to "handle" the message, which
    means to set the Handled flag of the message at some point in time.
    Setting the Handled flag doesn't have to happen within the handler function,
    it can also happen at some later time.
    
    The Dispatcher will never "own" the message, it only looks up and
    calls the handler function subscribed to a specific message.
*/
#include <functional>
#include "Messaging/Port.h"

namespace Oryol {

typedef std::function<void(const Ptr<Message>&)> HandlerFunc;

template<class PROTOCOL> class Dispatcher : public Port {
    OryolClassDecl(Dispatcher);
public:
    /// constructor
    Dispatcher();
    /// destructor
    ~Dispatcher();
    
    /// put a message into the port
    virtual bool Put(const Ptr<Message>& msg) override;
    
    /// bind a function to a message
    template<class MSG> void Subscribe(std::function<void(const Ptr<MSG>&)> func);
    /// unsubscribe from a specific message
    template<class MSG> void Unsubscribe();
    
private:
    HandlerFunc jumpTable[PROTOCOL::MessageId::NumMessageIds];
};

//------------------------------------------------------------------------------
template<class PROTOCOL>
Dispatcher<PROTOCOL>::Dispatcher() {
    // empty
}

//------------------------------------------------------------------------------
template<class PROTOCOL>
Dispatcher<PROTOCOL>::~Dispatcher() {
    // empty
}

//------------------------------------------------------------------------------
template<class PROTOCOL> bool
Dispatcher<PROTOCOL>::Put(const Ptr<Message>& msg) {
    // only consider messages of our protocol, ignore others
    if (msg->IsMemberOf(PROTOCOL::GetProtocolId())) {
    
        MessageIdType msgId = msg->MessageId();
        o_assert((msgId >= 0) && (msgId < PROTOCOL::MessageId::NumMessageIds));
        
        // check if a handler function has been set
        if (this->jumpTable[msgId]) {
            // call the handler function
            this->jumpTable[msgId](msg);
            return true;
        }
        else {
            o_warn("No message handler set for message id '%d'\n", msgId);
        }
    }
    return false;
}

//------------------------------------------------------------------------------
template<class PROTOCOL> template<class MSG> void
Dispatcher<PROTOCOL>::Subscribe(std::function<void(const Ptr<MSG>&)> func) {
    const MessageIdType classMsgId = MSG::ClassMessageId();
    o_assert((classMsgId >= 0) && (classMsgId < PROTOCOL::MessageId::NumMessageIds));
    
    /// @todo: is this safe?
    o_assert(sizeof(HandlerFunc) == sizeof(func));
    this->jumpTable[classMsgId] = *(HandlerFunc*)&func;
}

//------------------------------------------------------------------------------
template<class PROTOCOL> template<class MSG> void
Dispatcher<PROTOCOL>::Unsubscribe() {
    const MessageIdType classMsgId = MSG::ClassMessageId();
    o_assert((classMsgId >= 0) && (classMsgId < PROTOCOL::MessageId::NumMessageIds));
    this->jumpTable[classMsgId] = HandlerFunc();
}

} // namespace Oryol