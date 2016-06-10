#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputDispatcher
    @brief dispatch input module events
*/
#include "Input/Core/InputEnums.h"
#include "Input/Core/InputEvent.h"
#include "Core/Containers/Map.h"
#include <functional>

namespace Oryol {
namespace _priv {

class inputDispatcher {
public:
    /// input event callback function
    typedef std::function<void(const InputEvent&)> inputEventCallback;
    /// mouse pointer lock handler type
    typedef std::function<PointerLockMode::Code(const InputEvent&)> pointerLockCallback;
    /// typedef for input event callback id
    typedef int callbackId;

    /// subscribe to keyboard events
    callbackId subscribeEvents(inputEventCallback handler);
    /// unsubscribe from keyboard events
    void unsubscribeEvents(callbackId id);
    /// notify event handlers
    void notifyEvent(const InputEvent& event);
    /// notify pointer lock handler
    PointerLockMode::Code notifyPointerLock(const InputEvent& event);

    callbackId uniqueIdCounter = 0;
    Map<callbackId, inputEventCallback> inputEventHandlers;
    pointerLockCallback pointerLockHandler;
};

} // namespace _priv
} // namespace Oryol
