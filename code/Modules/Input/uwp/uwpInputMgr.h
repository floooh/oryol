#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::uwpInputMgr
    @ingroup _priv
    @brief provide input for the Windows UWP platform
*/
#include "Input/Core/inputMgrBase.h"
#include "Core/RunLoop.h"

namespace Oryol {
namespace _priv {

class uwpInputMgr : public inputMgrBase {
public:
    /// constructor
    uwpInputMgr();
    /// destructor
    ~uwpInputMgr();
    /// setup the window input manager
    void setup(const InputSetup& setup);
    /// discard the windows input manager
    void discard();

    /// called with new pointer lock, make mouse pointer visible/invisible here
    void onPointerLock(PointerLockMode::Code lockMode);
    /// map UWP virtual keycode to Oryol keycode
    static Key::Code mapKey(Windows::UI::Core::KeyEventArgs^ args);
    /// setup the key-translation table
    static void setupKeymap();

    static uwpInputMgr* ptr;
    RunLoop::Id runLoopId;
};

} // namespace _priv
} // namespace Oryol
