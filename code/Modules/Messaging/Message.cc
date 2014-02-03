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
uint8*
Message::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {
    // FIXME!
    return dstPtr;
}

//------------------------------------------------------------------------------
const uint8*
Message::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {
    // FIXME!
    return srcPtr;
}
    
} // namespace Messaging
} // namespace Oryol