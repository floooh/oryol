#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::iosInputMgr
    @brief input manager for iOS
*/
#include "Input/private/inputMgrBase.h"
#include "Core/private/ios/iosBridge.h"
#include "Core/RunLoop.h"

namespace Oryol {
namespace _priv {

class iosInputMgr : public inputMgrBase {
public:
    /// constructor
    iosInputMgr();
    /// destructor
    ~iosInputMgr();
    
    /// setup the object
    void setup(const InputSetup& setup);
    /// discard the object
    void discard();
    
    /// sample motion data
    void sampleMotionData();

    ORYOL_OBJC_ID inputDelegate;
    ORYOL_OBJC_ID motionManager;
    RunLoop::Id resetRunLoopId;
    RunLoop::Id motionRunLoopId;
};

} // namespace _priv
} // namespace Oryol
