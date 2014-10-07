#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::iosInputMgr
    @brief input manager for iOS
*/
#include "Input/base/inputMgrBase.h"
#include "Input/touch/touchEvent.h"
#include "Input/touch/tapDetector.h"
#include "Input/touch/panDetector.h"
#include "Input/touch/pinchDetector.h"
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
    
    /// called when a touch event occured
    void onTouchEvent(const touchEvent& touchEvent);
    
private:
    id inputDelegate;
    RunLoop::Id runLoopId;
    tapDetector singleTapDetector;
    tapDetector doubleTapDetector;
    class panDetector panDetector;
    class pinchDetector pinchDetector;
};

} // namespace _priv
} // namespace Oryol
