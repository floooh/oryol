#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::emscInputMgr
    @ingroup _priv
    @brief provide input on emscripten platform
*/
#include "Input/base/inputMgrBase.h"
#include "Input/touch/touchEvent.h"
#include "Input/touch/tapDetector.h"
#include "Input/touch/panDetector.h"
#include "Input/touch/pinchDetector.h"
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

    /// set mouse cursor mode
    void setCursorMode(CursorMode::Code mode);

private:
    /// setup the key mapping table
    void setupKeyTable();    
    /// setup emscripten input handling callbacks
    void setupCallbacks();
    /// detach emscripten callbacks
    void discardCallbacks();
    /// map HTML5 key code to Oryol keycode
    Key::Code mapKey(unsigned long html5KeyCode) const;
    /// map HTML5 mouse button code to Oryol mouse button
    Mouse::Button mapMouseButton(unsigned short html5Btn) const;

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
    /// pointerlockchange callback
    static EM_BOOL emscPointerLockChange(int eventType, const EmscriptenPointerlockChangeEvent* e, void* userData);
    /// touch event callback (same callback for touchstart, touchmove, touchend, touchcancel)
    static EM_BOOL emscTouch(int eventType, const EmscriptenTouchEvent* e, void* userData);
    
    static const int32 MaxNumKeys = 256;
    RunLoop::Id runLoopId;
    Key::Code keyTable[MaxNumKeys];
};

} // namespace _priv
} // namespace Oryol
