#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::emscInputMgr
    @ingroup _priv
    @brief provide input on emscripten platform
*/
#include "Input/private/inputMgrBase.h"
#include "Core/RunLoop.h"
#include <emscripten/html5.h>

namespace Oryol {
namespace _priv {

class emscInputMgr : public inputMgrBase {
public:
    /// constructor
    emscInputMgr();
    /// destructor
    ~emscInputMgr();

    /// setup the input manager
    void setup(const InputSetup& setup);
    /// discard the input manager
    void discard();

    /// setup game pad mappings for known gamepads
    void setupGamepadMappings();
    /// polling for the gamepad
    void updateGamepads();
    /// setup the key mapping table
    void setupKeyTable();    
    /// setup emscripten input handling callbacks
    void setupCallbacks();
    /// detach emscripten callbacks
    void discardCallbacks();
    /// map HTML5 key code to Oryol keycode
    Key::Code mapKey(unsigned long html5KeyCode) const;
    /// map HTML5 mouse button code to Oryol mouse button
    MouseButton::Code mapMouseButton(unsigned short html5Btn) const;

    /// update mouse pointer lock state
    static bool updatePointerLockMode(PointerLockMode::Code lockMode);
    /// key down callback
    static EM_BOOL emscKeyDown(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
    /// key up callback
    static EM_BOOL emscKeyUp(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
    /// key press callback
    static EM_BOOL emscKeyPress(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
    /// mouse-button-down callback
    static EM_BOOL emscMouseDown(int eventType, const EmscriptenMouseEvent* e, void* userData);
    /// mouse-button-up callback
    static EM_BOOL emscMouseUp(int eventType, const EmscriptenMouseEvent* e, void* userData);
    /// mouse-move callback
    static EM_BOOL emscMouseMove(int eventType, const EmscriptenMouseEvent* e, void* userData);
    /// wheel callback
    static EM_BOOL emscWheel(int eventType, const EmscriptenWheelEvent* e, void* userData);
    /// touch event callback (same callback for touchstart, touchmove, touchend, touchcancel)
    static EM_BOOL emscTouch(int eventType, const EmscriptenTouchEvent* e, void* userData);
    /// device motion callback
    static EM_BOOL emscDeviceMotion(int eventType, const EmscriptenDeviceMotionEvent* e, void* userData);
    /// device orientation callback
    static EM_BOOL emscDeviceOrientation(int eventType, const EmscriptenDeviceOrientationEvent* e, void* userData);

    static const int MaxNumKeys = 256;
    RunLoop::Id runLoopId;
    RunLoop::Id updateGamepadsRunLoopId;
    bool pointerLockActive;
    Key::Code keyTable[MaxNumKeys];
};

} // namespace _priv
} // namespace Oryol
