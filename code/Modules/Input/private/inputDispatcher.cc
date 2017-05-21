//------------------------------------------------------------------------------
//  inputDispatcher.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputDispatcher.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
inputDefs::callbackId
inputDispatcher::subscribeEvents(inputDefs::inputEventCallback handler) {
    inputDefs::callbackId id = ++this->uniqueIdCounter;
    this->inputEventHandlers.Add(id, handler);
    return id;
}

//------------------------------------------------------------------------------
void
inputDispatcher::unsubscribeEvents(inputDefs::callbackId id) {
    if (this->inputEventHandlers.Contains(id)) {
        this->inputEventHandlers.Erase(id);
    }
}

//------------------------------------------------------------------------------
void
inputDispatcher::notifyEvent(const InputEvent& ie) {
    for (const auto& entry : this->inputEventHandlers) {
        entry.Value()(ie);
    }
}

//------------------------------------------------------------------------------
PointerLockMode::Code
inputDispatcher::notifyPointerLock(const InputEvent& ie) {
    if (this->pointerLockHandler) {
        return this->pointerLockHandler(ie);
    }
    else {
        return PointerLockMode::DontCare;
    }
}

} // namespace _priv
} // namespace Oryol
