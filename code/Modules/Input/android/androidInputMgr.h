#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::androidInputMgr
    @brief input manager for Android
*/
#include "Input/base/inputMgrBase.h"
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
    /// callback method from global android_app object
    static int32_t onInputEvent(struct android_app* app, AInputEvent* event);

    RunLoop::Id runLoopId;
};

} // namespace _priv
} // namespace Oryol