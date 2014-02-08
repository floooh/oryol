//------------------------------------------------------------------------------
//  Broadcaster.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Broadcaster.h"

namespace Oryol {
namespace Messaging {
    
OryolClassPoolAllocImpl(Broadcaster);

using namespace Core;

//------------------------------------------------------------------------------
Broadcaster::Broadcaster(const StringAtom& name_) :
    Port(name_)
{
    // empty
}

//------------------------------------------------------------------------------
Broadcaster::~Broadcaster() {
    this->subscribers.Clear();
}

//------------------------------------------------------------------------------
void
Broadcaster::Put(const Ptr<Message>& msg) {
    for (const Ptr<Port>& sub : this->subscribers) {
        sub->Put(msg);
    }
}

//------------------------------------------------------------------------------
void
Broadcaster::Subscribe(const Ptr<Port>& port) {
    o_assert(InvalidIndex == this->subscribers.FindIndexLinear(port));
    this->subscribers.AddBack(port);
}
    
//------------------------------------------------------------------------------
void
Broadcaster::Unsubscribe(const Ptr<Port>& port) {
    int32 index = this->subscribers.FindIndexLinear(port);
    o_assert(InvalidIndex != index);
    this->subscribers.Erase(index);
}
    
//------------------------------------------------------------------------------
const Array<Ptr<Port>>&
Broadcaster::GetSubscribers() const {
    return this->subscribers;
}
    
} // namespace Broadcaster
} // namespace Oryol