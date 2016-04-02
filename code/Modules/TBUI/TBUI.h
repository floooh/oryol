#pragma once
//-----------------------------------------------------------------------------
/**
    @defgroup TBUI TBUI
    @brief TurboBadger UI Wrapper Module

    @class Oryol::TBUI
    @ingroup TBUI
    @brief facade class for Oryol TurboBadgerUI wrapper module
*/
#include "Core/Types.h"
#include "TBUI/tb/tbMgr.h"
#include "TBUI/tb/tbResourceContainer.h"
#include "TBUI/TBUISetup.h"

namespace tb {
class TBFile;
}

namespace Oryol {

namespace _priv {
class tbOryolRootWidget;
}

class TBUI {
public:
    /// DoAfter callback
    typedef std::function<void()> DoAfterFunc;

    /// setup the TurboBadger UI module
    static void Setup(const TBUISetup& setup);
    /// shutdown the TurboBadger UI module
    static void Discard();
    /// test if TurboBadger UI module has been setup
    static bool IsValid();

    /// preload a single resource, and then invoke callback
    static void DoAfter(const URL& loadUrl, DoAfterFunc func);
    /// preload resource group, and then invoke callback
    static void DoAfter(const Array<URL>& loadUrls, DoAfterFunc func);
    /// initialize TurboBadger, call after setup resources have been loaded
    static void InitTurboBadger();
    
    /// draw one frame, call this before Gfx::Commit
    static void Draw();
    
private:
    friend class TBUIWindow;
    friend class tb::TBFile;
    
    /// get pointer to root window
    static _priv::tbOryolRootWidget* getRootWidget();
    /// access to resource container object
    static _priv::tbResourceContainer& resources();

    struct _state {
        TBUISetup setup;
        _priv::tbMgr mgr;
        _priv::tbResourceContainer resourceContainer;
    };
    static _state* state;
};

//-----------------------------------------------------------------------------
inline bool
TBUI::IsValid() {
    return nullptr != state;
}

} // namespace Oryol
