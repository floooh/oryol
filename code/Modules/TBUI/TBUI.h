#pragma once
//-----------------------------------------------------------------------------
/**
    @class Oryol::TBUI
    @brief facade class for Oryol TurboBadgerUI wrapper module
*/
#include "Core/Types.h"
#include "TBUI/tb/tbMgr.h"
#include "TBUI/TBUISetup.h"
#include "TBUI/tb/TBUIResourceContainer.h"

namespace Oryol {

class TBUI {
public:
    /// setup the TurboBadger UI module
    static void Setup(const TBUISetup& setup);
    /// shutdown the TurboBadger UI module
    static void Discard();
    /// test if TurboBadger UI module has been setup
    static bool IsValid();
    
    /// access to resource container
    static TBUIResourceContainer& Resource();
    
private:
    struct _state {
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
