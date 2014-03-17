//------------------------------------------------------------------------------
//  Message.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Message.h"

namespace Oryol {
namespace Messaging {
    
OryolClassImpl(Message);

//------------------------------------------------------------------------------
Message::~Message() {
    // empty
}

//------------------------------------------------------------------------------
bool
Message::IsMemberOf(ProtocolIdType protId) const {
    return false;
}

//------------------------------------------------------------------------------
void
Message::SetHandled() {
    this->handled = true;
}

//------------------------------------------------------------------------------
void
Message::SetCancelled() {
    this->cancelled = true;
}

//------------------------------------------------------------------------------
bool
Message::Pending() const {
    return !this->handled;
}

//------------------------------------------------------------------------------
bool
Message::Handled() const {
    return this->handled;
}

//------------------------------------------------------------------------------
bool
Message::Cancelled() const {
    return this->cancelled;
}

//------------------------------------------------------------------------------
int32
Message::EncodedSize() const {
    /// @todo: this should include some sort of header information
    return 0;
}

//------------------------------------------------------------------------------
uint8*
Message::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    // @todo: this should write a header with the MessageId
    return dstPtr;
}

//------------------------------------------------------------------------------
const uint8*
Message::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    /// @todo: this should decode the header
    return srcPtr;
}
    
} // namespace Messaging
} // namespace Oryol