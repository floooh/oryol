#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::iosInputMgr
    @brief input manager for iOS
*/
#include "Input/base/inputMgrBase.h"
#include "Core/ios/iosBridge.h"
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
    
private:
    id inputDelegate;
    RunLoop::Id runLoopId;
};

} // namespace _priv
} // namespace Oryol
