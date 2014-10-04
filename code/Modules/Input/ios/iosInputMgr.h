#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::iosInputMgr
    @brief input manager for iOS
*/
#include "Input/base/inputMgrBase.h"
#include "Input/touch/touch.h"
#include "Input/touch/tapDetector.h"
#include "Input/touch/panDetector.h"
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
    void onTouchEvent(const touch& touchEvent);
    
private:
    /// per-frame callback to reset input state at start of frame
    void reset();
    /// transform coordinates to landscape
    glm::vec2 toLandscape(float32 x, float32 y);

    float32 screenWidth;
    float32 screenHeight;
    id inputDelegate;
    RunLoop::Id runLoopId;
    tapDetector singleTapDetector;
    tapDetector doubleTapDetector;
    class panDetector panDetector;
};

} // namespace _priv
} // namespace Oryol
