#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::raspiInputMgr
    @ingroup _priv
    @brief input manager for Raspberry Pi using /dev/input
*/
#include "Input/private/inputMgrBase.h"
#include "Core/RunLoop.h"

namespace Oryol {
namespace _priv {

class raspiInputMgr : public inputMgrBase {
public:
    /// constructor
    raspiInputMgr();
    /// destructor
    ~raspiInputMgr();

    /// setup the input manager
    void setup(const InputSetup& setup);
    /// discard the input manager
    void discard();

    /// open mouse and keyboard device file under /dev/input
    bool openDevices();
    /// close mouse and keyboard device
    void closeDevices();
    /// per-frame poll input 
    void pollInput();
    /// update max mouse coords, this depends on display resolution
    void updateMaxMouseCoords();
    /// called on key event
    void onKey(unsigned short code, int val);
    /// called when mouse button is pressed
    void onMouseButton(unsigned short code, int val);
    /// called when mouse moved
    void onMouseMove(unsigned short code, int val);
    
    /// translate raw Linux key code to Oryol key code
    Key::Code mapKey(unsigned short code);
    /// translate raw Linux key code to ASCII wchar_t (hardwired)
    wchar_t mapChar(unsigned short code, bool shift);

    RunLoop::Id runLoopId;
    int kbdFd;
    int mouseFd;
    bool leftShift;
    bool rightShift;

    static const int mouseSensitivity = 1;
    int maxMouseX;
    int maxMouseY;
    int curMouseX;
    int curMouseY;
};

} // namespace _priv
} // namespace Oryol
