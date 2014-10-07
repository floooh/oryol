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
#include "Input/Core/CursorMode.h"
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

    /// get keyboard state
    const class Keyboard& Keyboard() const;
    /// get mouse state
    const class Mouse& Mouse() const;
    /// get gamepad state
    const class Gamepad& Gamepad(int32 index) const;
    /// get touchpad state (for touch/multitouch gestures)
    const class Touchpad& Touchpad() const;
    
    /// set mouse cursor mode
    void setCursorMode(CursorMode::Code mode);
    /// get mouse cursor mode
    CursorMode::Code getCursorMode() const;

    /// begin text capturing
    void beginCaptureText();
    /// end text capturing
    void endCaptureText();

    /// handle a touch event (detect gestures)
    void onTouchEvent(const touchEvent& event);
    
protected:
    static const int32 MaxNumGamepads = 4;
    bool valid;
    InputSetup inputSetup;
    class Keyboard keyboard;
    class Mouse mouse;
    class Gamepad gamepads[MaxNumGamepads];
    class Touchpad touchpad;
    tapDetector singleTapDetector;
    tapDetector doubleTapDetector;
    class panDetector panDetector;
    class pinchDetector pinchDetector;        
    CursorMode::Code cursorMode;    
};

//------------------------------------------------------------------------------
inline const class Keyboard&
inputMgrBase::Keyboard() const {
    return this->keyboard;
}

//------------------------------------------------------------------------------
inline const class Mouse&
inputMgrBase::Mouse() const {
    return this->mouse;
}

//------------------------------------------------------------------------------
inline const class Gamepad&
inputMgrBase::Gamepad(int32 index) const {
    o_assert_range(index, MaxNumGamepads);
    return this->gamepads[index];
}

//------------------------------------------------------------------------------
inline const class Touchpad&
inputMgrBase::Touchpad() const {
    return this->touchpad;
}

//------------------------------------------------------------------------------
inline CursorMode::Code
inputMgrBase::getCursorMode() const {
    return this->cursorMode;
}

} // namespace _priv
} // namespace Oryol



