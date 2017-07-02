#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::androidInputMgr
    @brief input manager for Android
*/
#include "Input/private/inputMgrBase.h"
#include "Core/private/android/androidBridge.h"
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

    /// callback method from global android_app object
    static int32_t onInputEvent(struct android_app* app, AInputEvent* event);
    /// sensor event callback
    static void onSensorEvent(const ASensorEvent* event);

    RunLoop::Id runLoopId;
    bool highDPI;
};

} // namespace _priv
} // namespace Oryol
