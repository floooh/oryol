#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::androidBrigde
    @ingroup _priv
    @brief Android bridge class between Oryol app and native activity glue
*/
#include "Core/Types.h"
#include "android/sensor.h"
#include <functional>

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
    /// return ptr to global androidBridge object
    static androidBridge* ptr();    
    /// setup the bridge with pointer to App object
    void setup(App* app);
    /// discard the object
    void discard();
    /// return true if the object has been setup
    bool isValid() const;
    /// set sensor event callback
    void setSensorEventCallback(std::function<void(const ASensorEvent*)> cb);
    /// called in App::StartMainLoop
    void onStart();
    /// called in a loop by App::StartMainLoop
    bool onFrame();
    /// called at end of start main loop
    void onStop();

    static void onAppCmd(struct android_app* app, int32_t cmd);

    static androidBridge* self;
    bool valid;
    bool hasWindow;
    bool hasFocus;
    App* app;
    ASensorManager* sensorManager;
    const ASensor* accelSensor;
    const ASensor* gyroSensor;
    ASensorEventQueue* sensorEventQueue;    
    std::function<void(const ASensorEvent*)> sensorEventCallback;
};
} // namespace _priv
} // namespace Oryol
