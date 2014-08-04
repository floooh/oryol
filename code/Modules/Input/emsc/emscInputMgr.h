#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::emscInputMgr
    @brief provide input on emscripten platform
*/
#include "Input/base/inputMgrBase.h"
#include <emscripten/html5.h>

namespace Oryol {
namespace Input {

class emscInputMgr : public inputMgrBase {
public:
    /// constructor
    emscInputMgr();
    /// destructor
    ~emscInputMgr();

    /// set mouse cursor mode
    void setCursorMode(CursorMode::Code mode);

private:
    /// per-frame callback to reset input state at start of frame
    void reset();
    /// setup the key mapping table
    void setupKeyTable();    
    /// setup emscripten input handling callbacks
    void setupCallbacks();
    /// detach emscripten callbacks
    void discardCallbacks();
    /// map HTML5 key code to Oryol keycode
    Key::Code mapKey(unsigned long html5KeyCode) const;

    /// key down callback
    static EM_BOOL emscKeyDown(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
    /// key up callback
    static EM_BOOL emscKeyUp(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
    /// key press callback
    static EM_BOOL emscKeyPress(int eventType, const EmscriptenKeyboardEvent* e, void* userData);

    static const int32 MaxNumKeys = 256;
    Key::Code keyTable[MaxNumKeys];
};

} // namespace Input
} // namespace Oryol