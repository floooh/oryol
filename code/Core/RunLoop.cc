//------------------------------------------------------------------------------
//  RunLoop.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RunLoop.h"

namespace Oryol {
namespace Core {

OryolClassImpl(RunLoop);

//------------------------------------------------------------------------------
RunLoop::RunLoop() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
RunLoop::~RunLoop() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
bool
RunLoop::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
RunLoop::Setup() {
    o_assert(!this->isValid);
    o_assert(this->callbacks.Empty());
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
RunLoop::Discard() {
    o_assert(this->isValid);
    this->callbacks.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
RunLoop::Run() {
    o_assert(this->isValid);
    this->AddCallbacks();
    for (const auto& entry : this->callbacks) {
        const Callback& cb = entry.Value();
        if (cb.IsValid()) {
            cb.Func()();
        }
    }
    this->RemoveCallbacks();
}

//------------------------------------------------------------------------------
bool
RunLoop::HasCallback(const StringAtom& name) const {
    o_assert(this->isValid);
    return this->FindCallback(name);
}

//------------------------------------------------------------------------------
int32
RunLoop::FindCallback(const StringAtom& name) const {
    const int32 num = this->callbacks.Size();
    for (int32 i = 0; i < num; i++) {
        if (this->callbacks[i].Name() == name) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
/**
 Add a new callback to the runloop. Callbacks with smaller priority
 values are called earlier in the frame. The callback name must be unique.
 NOTE that the callback will not be added immediately, but only at the 
 beginning of the next Run() call.
*/
void
RunLoop::Add(const Callback& callback) {
    o_assert(this->isValid);
    o_assert(!this->toAdd.Contains(callback.Name()));
    o_assert(InvalidIndex == this->FindCallback(callback.Name()));
    
    this->toAdd.Insert(callback.Name(), callback);
}

//------------------------------------------------------------------------------
/**
 Returns a callback from the RunLoop. NOTE that the callback will not be removed
 immediately, but at the end of the current or next Run() call (it will not
 however called after Remove is called).
*/
void
RunLoop::Remove(const StringAtom& name) {
    o_assert(this->isValid);
    o_assert(!this->toRemove.Contains(name));
    
    int32 index = this->FindCallback(name);
    o_assert(InvalidIndex != index);
    this->callbacks[index].SetValid(false);
    this->toRemove.Insert(name);
}

//------------------------------------------------------------------------------
void
RunLoop::AddCallbacks() {
    for (auto& entry : this->toAdd) {
        Callback& cb = entry.Value();
        cb.SetValid(true);
        this->callbacks.Insert(cb.Priority(), cb);
    }
    this->toAdd.Clear();
}

//------------------------------------------------------------------------------
void
RunLoop::RemoveCallbacks() {
    for (const StringAtom& name : this->toRemove) {
        int32 index = this->FindCallback(name);
        if (InvalidIndex != index) {
            o_assert(!this->callbacks[index].IsValid());
            this->callbacks.EraseIndex(index);
        }
    }
}

//------------------------------------------------------------------------------
const Map<int32, RunLoop::Callback>&
RunLoop::Callbacks() const {
    return this->callbacks;
}

} // namespace Core
} // namespace Oryol