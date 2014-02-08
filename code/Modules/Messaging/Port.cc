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
Port::Port(const StringAtom& name_) :
name(name_) {
    // empty
}

//------------------------------------------------------------------------------
Port::~Port() {
    // empty
}

//------------------------------------------------------------------------------
void
Port::Put(const Ptr<Message>& msg) {
    // empty, override in subclass as needed
}

//------------------------------------------------------------------------------
const StringAtom&
Port::GetName() const {
    return this->name;
}

} // namespace Messaging
} // namespace Oryol