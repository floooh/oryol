//------------------------------------------------------------------------------
//  RunLoop.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RunLoop.h"

namespace Oryol {

//------------------------------------------------------------------------------
RunLoop::RunLoop() :
curId(InvalidId)
{
    // empty
}

//------------------------------------------------------------------------------
RunLoop::~RunLoop() {
    // empty
}

//------------------------------------------------------------------------------
void
RunLoop::Run() {
    this->remCallbacks();
    this->addCallbacks();
    for (const auto& entry : this->callbacks) {
        if (entry.Value().valid) {
            entry.Value().func();
        }
    }
    this->remCallbacks();
    this->addCallbacks();
}

//------------------------------------------------------------------------------
bool
RunLoop::HasCallback(Id id) const {
    return this->callbacks.Contains(id) || this->toAdd.Contains(id);
}

//------------------------------------------------------------------------------
/**
 NOTE: the callback function will not be added immediately, but at the
 start or end of the Run function.
*/
RunLoop::Id
RunLoop::Add(Func func) {
    Id newId = ++this->curId;
    this->toAdd.Add(newId, item{func, false});
    return newId;
}

//------------------------------------------------------------------------------
/**
 NOTE: the callback function not be removed immediately, but at the 
 start or end of the Run function.
*/
void
RunLoop::Remove(Id id) {
    o_assert_dbg(!this->toRemove.Contains(id));
    o_assert_dbg(this->callbacks.Contains(id) || this->toAdd.Contains(id));
    this->toRemove.Add(id);
}

//------------------------------------------------------------------------------
void
RunLoop::addCallbacks() {
    for (auto& entry : this->toAdd) {
        item& item = entry.Value();
        item.valid = true;
        this->callbacks.Add(entry.Key(), item);
    }
    this->toAdd.Clear();
}

//------------------------------------------------------------------------------
void
RunLoop::remCallbacks() {
    for (Id id : this->toRemove) {
        if (this->callbacks.Contains(id)) {
            this->callbacks.Erase(id);
        }
        else if (this->toAdd.Contains(id)) {
            this->toAdd.Erase(id);
        }
    }
    this->toRemove.Clear();
}

} // namespace Oryol
