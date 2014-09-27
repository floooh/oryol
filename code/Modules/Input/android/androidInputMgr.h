#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::androidInputMgr
    @brief input manager for Android
*/
#include "Core/RunLoop.h"
#include "Input/base/inputMgrBase.h"
#include "Core/android/androidBridge.h"
#include "android_native/gestureDetector.h"

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

    RunLoop::Id runLoopId;
    ndk_helper::TapDetector tapDetector;
    ndk_helper::DoubletapDetector doubleTapDetector;
    ndk_helper::PinchDetector pinchDetector;
    ndk_helper::DragDetector dragDetector;
};

} // namespace _priv
} // namespace Oryol