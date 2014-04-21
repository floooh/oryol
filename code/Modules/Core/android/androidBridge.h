#pragma once
//------------------------------------------------------------------------------
/**
    @class Core::androidBrigde
    @brief Android bridge class between Oryol app and native activity glue
*/
#include "Core/Types.h"

struct android_app;

namespace Oryol {
namespace Core {
class AppBase;

class androidBridge {
public:
    /// constructor
    androidBridge();
    /// destructor
    ~androidBridge();
    /// setup the bridge with pointer to AppBase object
    void setup(AppBase* appBase);
    /// discard the object
    void discard();
    /// return true if the object has been setup
    bool isValid() const;
    /// called in AppBase::StartMainLoop
    void onStart();
    /// called in a loop by AppBase::StartMainLoop
    void onFrame();
    /// called at end of start main loop
    void onStop();

private:
    static void onAppCmd(struct android_app* app, int32_t cmd);

    bool valid;
    bool hasWindow;
    bool hasFocus;
    AppBase* appBase;
};
} // namespace Core
} // namespace Oryol
