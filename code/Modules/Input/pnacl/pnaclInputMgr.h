#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pnaclInputMgr
    @brief PNaCl platform input manager class
*/
#include "Input/base/inputMgrBase.h"
#include "ppapi/cpp/input_event.h"
#include "Core/RunLoop.h"

namespace Oryol {
namespace _priv {

class pnaclInputMgr : public inputMgrBase {
public:
    /// constructor
    pnaclInputMgr();
    /// destructor
    ~pnaclInputMgr();

    /// setup the input manager
    void setup(const InputSetup& setup);
    /// discard the input manager
    void discard();

private:
    /// setup the key code mapping table
    void setupKeyTable();
    /// convert nacl key code to oryol key code
    Key::Code mapKey(uint32_t naclKeyCode) const;
    /// map NaCl mouse button to Oryol mouse button
    Mouse::Button mapMouseButton(PP_InputEvent_MouseButton naclBtn);
    /// input event callback from pnaclInstance::HandleInputEvent
    bool handleEvent(const pp::InputEvent& ie);
    /// called on mouse-down event from handleEvent
    bool onMouseDown(const pp::MouseInputEvent& ie);
    /// called on mouse-up event from handleEvent
    bool onMouseUp(const pp::MouseInputEvent& ie);
    /// called on mouse-move event from handleEvent
    bool onMouseMove(const pp::MouseInputEvent& ie);
    /// called on mouse-enter event from handleEvent
    bool onMouseEnter(const pp::MouseInputEvent& ie);
    /// called on mouse-leave event from handleEvent
    bool onMouseLeave(const pp::MouseInputEvent& ie);
    /// called on mouse wheel event from handleEvent
    bool onWheel(const pp::WheelInputEvent& ie);
    /// called on key-down event from handleEvent
    bool onKeyDown(const pp::KeyboardInputEvent& ie);
    /// called on key-up event from handleEvent
    bool onKeyUp(const pp::KeyboardInputEvent& ie);
    /// called on char event from handleEvent
    bool onChar(const pp::KeyboardInputEvent& ie);

    static const int MaxNumKeys = 256;
    RunLoop::Id runLoopId;    
    Key::Code keyTable[MaxNumKeys];    
};

} // namespace _priv
} // namespace Oryol
