#pragma
//-----------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbMgr.h
    @brief global TurboBadger UI wrapper
*/
#include "Core/Types.h"
#include "Core/RunLoop.h"
#include "IO/Core/IOQueue.h"
#include "TBUI/TBUISetup.h"
#include "TBUI/tb/tbOryolRenderer.h"
#include "TBUI/tb/tbOryolRootWidget.h"
#include "Resource/Core/resourceRegistry.h"
#include "tb_skin.h"

namespace Oryol {
namespace _priv {

class tbMgr {
public:
    /// destructor
    ~tbMgr();
    
    /// setup the object
    void Setup(const TBUISetup& setup);
    /// discard the object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// get pointer to root widget
    tbOryolRootWidget* GetRootWidget();
    
private:
    /// per-frame update
    void update();

    bool isValid;
    RunLoop::Id runLoopId = RunLoop::InvalidId;
    tbOryolRenderer* renderer = nullptr;
    tbOryolRootWidget rootWidget;
};

} // namespace _priv
} // namespace Oryol

