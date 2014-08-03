#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::InputFacade
    @brief facade singleton of the Oryol Input module
    
    The InputFacade singleton provides access to connected input devices,
    like keyboard, mouse and game pads. On mobile platforms it also allows
    to query touch gestures.
*/
#include "Core/Singleton.h"
#include "Input/Core/inputMgr.h"

namespace Oryol {
namespace Input {
    
class InputFacade {
    OryolLocalSingletonDecl(InputFacade);
public:
    /// constructor
    InputFacade();
    /// destructor
    ~InputFacade();
    
    /// get keyboard state
    const class Keyboard& Keyboard() const;
    /// get mouse state
    const class Mouse& Mouse() const;
    /// get gamepad state
    const class Gamepad& Gamepad(int32 index) const;
    /// get touchpad state (for touch/multitouch gestures)
    const class Touchpad& Touchpad() const;
    
    /// set mouse cursor mode
    void SetCursorMode(CursorMode::Code mode);
    /// get current mouse cursor mode
    CursorMode::Code GetCursorMode() const;
    /// begin text capturing
    void BeginCaptureText();
    /// end text capturing
    void EndCaptureText();
    
private:
    inputMgr inputManager;
};

} // namespace Input
} // namespace Oryol