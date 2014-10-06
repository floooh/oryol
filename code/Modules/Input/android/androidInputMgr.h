#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::androidInputMgr
    @brief input manager for Android
*/
#include "Input/base/inputMgrBase.h"
#include "Input/touch/touchEvent.h"
#include "Input/touch/tapDetector.h"
#include "Input/touch/panDetector.h"
#include "Input/touch/pinchDetector.h"
#include "Core/android/androidBridge.h"
#include "Core/RunLoop.h"

class android_app;
class AInputEvent;

namespace Oryol {
namespace _priv {

class androidInputMgr : public inputMgrBase {
public:
    /// constructor
    androidInputMgr();
    /// destructor
    ~androidInputMgr();

    /// setup the input manager
    void setup(const InputSetup& setup);
    /// discard the input manager
    void discard();

private:
    /// per-frame callback to reset input state at start of frame
    void reset();
    /// callback method from global android_app object
    static int32_t onInputEvent(struct android_app* app, AInputEvent* event);
    /// handle a touch event
    void handleTouchEvent(const touchEvent& event);

    RunLoop::Id runLoopId;
    tapDetector singleTapDetector;
    tapDetector doubleTapDetector;
    class panDetector panDetector;
    class pinchDetector pinchDetector;
};

} // namespace _priv
} // namespace Oryol