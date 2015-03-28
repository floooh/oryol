#pragma
//-----------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbMgr.h
    @brief global TurboBadger UI wrapper
*/
#include "Core/Types.h"
#include "Core/RunLoop.h"
#include "TBUI/tb/tbOryolRenderer.h"
#include "TBUI/tb/tbOryolRootWidget.h"

namespace Oryol {
namespace _priv {

class tbMgr {
public:
    /// constructor
    tbMgr();
    /// destructor
    ~tbMgr();
    
private:
    /// per-frame update
    void update();

    RunLoop::Id runLoopId = RunLoop::InvalidId;
    tbOryolRenderer renderer;
    tbOryolRootWidget rootWidget;
};

} // namespace _priv
} // namespace Oryol

