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
int32
Message::EncodedSize() const {
    // FIXME!
    return 0;
}

//------------------------------------------------------------------------------
int32
Message::Encode(uint8* ptr, int32 maxBytes) const {
    // FIXME!
    return 0;
}

//------------------------------------------------------------------------------
int32
Message::Decode(const uint8* ptr, int32 maxBytes) {
    // FIXME!
    return 0;
}
    
} // namespace Messaging
} // namespace Oryol