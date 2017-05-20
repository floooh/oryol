#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputDispatcher
    @brief dispatch input module events
*/
#include "Input/Core/InputTypes.h"
#include "Core/Containers/Map.h"
#include <functional>

namespace Oryol {
namespace _priv {

class inputDispatcher {
public:
    typedef std::function<void(const InputEvent&)> inputEventCallback;
    typedef std::function<PointerLockMode::Code(const InputEvent&)> pointerLockCallback;
    typedef int callbackId;

    callbackId subscribeEvents(inputEventCallback handler);
    void unsubscribeEvents(callbackId id);
    void notifyEvent(const InputEvent& event);
    PointerLockMode::Code notifyPointerLock(const InputEvent& event);

    callbackId uniqueIdCounter = 0;
    Map<callbackId, inputEventCallback> inputEventHandlers;
    pointerLockCallback pointerLockHandler;
};

} // namespace _priv
} // namespace Oryol
