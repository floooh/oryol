#pragma once
//-----------------------------------------------------------------------------
/**
    @class Oryol::TBUI
    @brief facade class for Oryol TurboBadgerUI wrapper module
*/
#include "Core/Types.h"
#include "TBUI/tb/tbMgr.h"
#include "TBUI/tb/TBUIResourceContainer.h"
#include "TBUI/TBUISetup.h"

namespace Oryol {

namespace _priv {
class tbOryolRootWidget;
}

class TBUI {
public:
    /// setup the TurboBadger UI module
    static void Setup(const TBUISetup& setup);
    /// shutdown the TurboBadger UI module
    static void Discard();
    /// test if TurboBadger UI module has been setup
    static bool IsValid();

    /// resource container access
    static TBUIResourceContainer& Resource();
    
    /// initialize TurboBadger, call after setup resources have been loaded
    static void InitTurboBadger();
    /// draw one frame, call this before Gfx::Commit
    static void Draw();
    
private:
    friend class TBUIWindow;
    
    /// get pointer to root window
    static _priv::tbOryolRootWidget* getRootWidget();

    struct _state {
        TBUISetup setup;
        _priv::tbMgr mgr;
        TBUIResourceContainer resourceContainer;
    };
    static _state* state;
};

//-----------------------------------------------------------------------------
inline bool
TBUI::IsValid() {
    return nullptr != state;
}

} // namespace Oryol
