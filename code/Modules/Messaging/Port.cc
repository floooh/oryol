//------------------------------------------------------------------------------
//  Port.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Port.h"

namespace Oryol {
namespace Messaging {
    
OryolClassImpl(Port);
    
using namespace Core;

//------------------------------------------------------------------------------
Port::Port() {
    // empty
}

//------------------------------------------------------------------------------
Port::~Port() {
    // empty
}

//------------------------------------------------------------------------------
bool
Port::Put(const Ptr<Message>& msg) {
    return false;
}

//------------------------------------------------------------------------------
void
Port::DoWork() {
    // empty, override in subclass
}

} // namespace Messaging
} // namespace Oryol