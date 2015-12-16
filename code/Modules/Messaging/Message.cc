//------------------------------------------------------------------------------
//  Message.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Message.h"

namespace Oryol {
    
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

} // namespace Oryol
