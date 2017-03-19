#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputMgrBase
    @ingroup _priv
    @brief base class for platform-specific input managers
*/
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "Input/Core/InputSetup.h"
#include "Input/Core/InputEvent.h"
#include "Input/Core/keyboardDevice.h"
#include "Input/Core/mouseDevice.h"
#include "Input/Core/gamepadDevice.h"
#include "Input/Core/touchpadDevice.h"
#include "Input/Core/sensorDevice.h"
#include "Input/touch/touchEvent.h"
#include "Input/touch/tapDetector.h"
#include "Input/touch/panDetector.h"
#include "Input/touch/pinchDetector.h"
#include "Input/Core/inputDispatcher.h"
#include "Input/Core/GamepadMapping.h"

namespace Oryol {
namespace _priv {
    
class inputMgrBase {
public:
    /// constructor
    inputMgrBase();
    /// destructor
    ~inputMgrBase();
    
    /// setup the input manager
    void setup(const InputSetup& setup);
    /// discard the input manager
    void discard();
    /// return true if the input manager has been setup
    bool isValid() const;
    /// reset input devices (usually called by RunLoop at end of frame)
    void reset();
    /// get the input setup object
    const InputSetup& getInputSetup() const;
    /// add a gamepad mapping ("__default" to set default mapping)
    void addGamepadMapping(const StringAtom& id, const GamepadMapping& mapping);
    /// lookup a gamepad mapping by id (return "__default" mapping if not found)
    const GamepadMapping& lookupGamepadMapping(const StringAtom& id) const;

    class keyboardDevice keyboard;
    class mouseDevice mouse;
    class touchpadDevice touchpad;
    class sensorDevice sensors;
    static const int MaxNumGamepads = 4;
    StaticArray<class gamepadDevice, MaxNumGamepads> gamepad;

    /// handle a touch event (detect gestures)
    void onTouchEvent(const touchEvent& event);

    bool valid;
    InputSetup inputSetup;
    inputDispatcher dispatcher;
    tapDetector singleTapDetector;
    tapDetector doubleTapDetector;
    class panDetector panDetector;
    class pinchDetector pinchDetector;
    GamepadMapping defaultGamepadMapping;
    Map<StringAtom, GamepadMapping> gamepadMappings;
};

} // namespace _priv
} // namespace Oryol



