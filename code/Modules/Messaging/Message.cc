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
int32
Message::EncodedSize() const {
    // @fixme
    return 0;
}

//------------------------------------------------------------------------------
uint8*
Message::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    // @fixme
    return dstPtr;
}

//------------------------------------------------------------------------------
const uint8*
Message::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    // @fixme
    return srcPtr;
}
    
} // namespace Messaging
} // namespace Oryol