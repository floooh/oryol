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
const StringAtom&
Port::GetName() const {
    return this->name;
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