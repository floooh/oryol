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
bool
Port::Put(const Ptr<Message>& msg) {
    return false;
}

//------------------------------------------------------------------------------
const StringAtom&
Port::GetName() const {
    return this->name;
}

} // namespace Messaging
} // namespace Oryol