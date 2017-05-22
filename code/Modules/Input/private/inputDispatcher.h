#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputDispatcher
    @brief dispatch input module events
*/
#include "Input/InputTypes.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace _priv {

class inputDispatcher {
public:
    inputDefs::callbackId subscribeEvents(inputDefs::inputEventCallback handler);
    void unsubscribeEvents(inputDefs::callbackId id);
    void notifyEvent(const InputEvent& event);
    PointerLockMode::Code notifyPointerLock(const InputEvent& event);

    inputDefs::callbackId uniqueIdCounter = 0;
    Map<inputDefs::callbackId, inputDefs::inputEventCallback> inputEventHandlers;
    inputDefs::pointerLockCallback pointerLockHandler;
};

} // namespace _priv
} // namespace Oryol
