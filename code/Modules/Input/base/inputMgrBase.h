#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputMgrBase
    @ingroup _priv
    @brief base class for platform-specific input managers
*/
#include "Input/Core/InputSetup.h"
#include "Input/Core/Keyboard.h"
#include "Input/Core/Mouse.h"
#include "Input/Core/Gamepad.h"
#include "Input/Core/Touchpad.h"
#include "Input/Core/Sensors.h"
#include "Input/touch/touchEvent.h"
#include "Input/touch/tapDetector.h"
#include "Input/touch/panDetector.h"
#include "Input/touch/pinchDetector.h"

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

    /// max number of gamepads
    static const int MaxNumGamepads = 4;
    /// keyboard state
    class Keyboard Keyboard;
    /// mouse state
    class Mouse Mouse;
    /// gamepad state
    StaticArray<class Gamepad, MaxNumGamepads> Gamepad;
    /// touchpad state
    class Touchpad Touchpad;
    /// sensor state
    class Sensors Sensors;

    /// begin text capturing
    void beginCaptureText();
    /// end text capturing
    void endCaptureText();

    /// handle a touch event (detect gestures)
    void onTouchEvent(const touchEvent& event);
    
protected:
    bool valid;
    InputSetup inputSetup;
    tapDetector singleTapDetector;
    tapDetector doubleTapDetector;
    class panDetector panDetector;
    class pinchDetector pinchDetector;        
};

} // namespace _priv
} // namespace Oryol



