#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::androidBrigde
    @ingroup _priv
    @brief Android bridge class between Oryol app and native activity glue
*/
#include "Core/Types.h"

struct android_app;

namespace Oryol {
class App;

namespace _priv {
class androidBridge {
public:
    /// constructor
    androidBridge();
    /// destructor
    ~androidBridge();
    /// setup the bridge with pointer to App object
    void setup(App* app);
    /// discard the object
    void discard();
    /// return true if the object has been setup
    bool isValid() const;
    /// called in App::StartMainLoop
    void onStart();
    /// called in a loop by App::StartMainLoop
    bool onFrame();
    /// called at end of start main loop
    void onStop();

private:
    static void onAppCmd(struct android_app* app, int32_t cmd);

    bool valid;
    bool hasWindow;
    bool hasFocus;
    App* app;
};
} // namespace _priv
} // namespace Oryol
