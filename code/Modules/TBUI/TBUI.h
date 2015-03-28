#pragma once
//-----------------------------------------------------------------------------
/**
    @class Oryol::TBUI
    @brief facade class for Oryol TurboBadgerUI wrapper module
*/
#include "Core/Types.h"
#include "TBUI/tb/tbMgr.h"

namespace Oryol {

class TBUI {
public:
    /// setup the TurboBadger UI module
    static void Setup();
    /// shutdown the TurboBadger UI module
    static void Discard();
    /// test if TurboBadger UI module has been setup
    static bool IsValid();
    
private:
    struct _state {
        _priv::tbMgr mgr;
    };
    static _state* state;
};

//-----------------------------------------------------------------------------
inline bool
TBUI::IsValid() {
    return nullptr != state;
}

} // namespace Oryol
