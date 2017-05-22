#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputMgrBase
    @ingroup _priv
    @brief base class for platform-specific input managers
*/
#include "Input/InputTypes.h"
#include "Input/private/inputDevices.h"
#include "Input/private/gestureDetectors.h"
#include "Input/private/inputDispatcher.h"

namespace Oryol {
namespace _priv {
    
class inputMgrBase {
public:
    inputMgrBase();
    ~inputMgrBase();
    
    void setup(const InputSetup& setup);
    void discard();
    bool isValid() const;
    void reset();
    void addGamepadMapping(const StringAtom& id, const GamepadMapping& mapping);
    const GamepadMapping& lookupGamepadMapping(const StringAtom& id) const;
    void onTouchEvent(const touchEvent& event);

    class keyboardDevice keyboard;
    class mouseDevice mouse;
    class touchpadDevice touchpad;
    class sensorDevice sensors;
    StaticArray<class gamepadDevice, inputDefs::maxNumGamepads> gamepad;


    bool valid = false;
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



