//------------------------------------------------------------------------------
//  androidBridge.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "androidBridge.h"
#include "Core/App.h"
#include "Core/Log.h"
#include "android_native_app_glue.h"

// this is in the app's main file (see App.h -> OryolApp)
extern android_app* OryolAndroidAppState;

namespace Oryol {
namespace _priv {    

androidBridge* androidBridge::self = nullptr;

//------------------------------------------------------------------------------
androidBridge::androidBridge() :
valid(false),
hasWindow(false),
hasFocus(false),
app(nullptr),
sensorManager(nullptr),
accelSensor(nullptr),
gyroSensor(nullptr),
sensorEventQueue(nullptr) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
androidBridge::~androidBridge() {
    o_assert(!this->isValid());
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
androidBridge*
androidBridge::ptr() {
    o_assert_dbg(nullptr != self);
    return self;
}

//------------------------------------------------------------------------------
void
androidBridge::setup(App* app_) {
    Log::Info("androidBridge::setup(): %p\n", this);
    o_assert(!this->isValid());
    o_assert(nullptr != app_);
    o_assert(nullptr != OryolAndroidAppState);
    this->app = app_;
    this->hasWindow = false;
    this->hasFocus = false;
    this->valid = true;
}

//------------------------------------------------------------------------------
void
androidBridge::discard() {
    o_assert(this->isValid());
    this->valid = false;
    this->app = nullptr;
    Log::Info("androidBridge::discard()\n");
}

//------------------------------------------------------------------------------
bool
androidBridge::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
androidBridge::setSensorEventCallback(std::function<void(const ASensorEvent*)> cb) {
    this->sensorEventCallback = cb;
}

//------------------------------------------------------------------------------
void
androidBridge::onStart() {
    o_assert(this->isValid());
    Log::Info("androidBridge::onStart()\n");

    // setup the callback hook for AppCmds
    OryolAndroidAppState->userData = this;
    OryolAndroidAppState->onAppCmd = androidBridge::onAppCmd;

    // setup the accelerometer
    this->sensorManager = ASensorManager_getInstance();
    this->accelSensor = ASensorManager_getDefaultSensor(this->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    this->gyroSensor = ASensorManager_getDefaultSensor(this->sensorManager, ASENSOR_TYPE_GAME_ROTATION_VECTOR);
    this->sensorEventQueue = ASensorManager_createEventQueue(this->sensorManager, OryolAndroidAppState->looper, LOOPER_ID_USER, NULL, NULL);
}

//------------------------------------------------------------------------------
bool
androidBridge::onFrame() {
    o_assert(this->isValid());
    o_assert(this->app);

    // poll all pending application events for this frame
    int id;
    int events;
    android_poll_source* source;
    while (0 <= (id = ALooper_pollAll(this->hasWindow ? 0 : 100, NULL, &events, (void**) &source))) {
        if (source) {
            source->process(OryolAndroidAppState, source);
        }

        // sensor event?
        if (id == LOOPER_ID_USER) {
            if (this->sensorEventQueue) {
                ASensorEvent event;
                while (ASensorEventQueue_getEvents(this->sensorEventQueue, &event, 1) > 0) {
                    if (this->sensorEventCallback) {
                        this->sensorEventCallback(&event);
                    }
                }
            }
        }
    }
    this->app->onFrame();
    return 0 == OryolAndroidAppState->destroyRequested;
}

//------------------------------------------------------------------------------
void
androidBridge::onStop() {
    o_assert(this->isValid());

    ASensorManager_destroyEventQueue(this->sensorManager, this->sensorEventQueue);
    this->sensorEventQueue = nullptr;    
    this->accelSensor = nullptr;
    this->gyroSensor = nullptr;
    this->sensorManager = nullptr;

    Log::Info("androidBridge::onStop()\n");
}

//------------------------------------------------------------------------------
/**
    FIXME: App should support some sort of callback mechanism
    (not Messages, too highlevel for Core), which notifies the
    higher parts about these App lifetime events!!
*/
void
androidBridge::onAppCmd(android_app* appState, int32_t cmd) {
    androidBridge* self = (androidBridge*) appState->userData;
    o_assert(self && self->isValid());
    switch (cmd) {
        /**
         * Command from main thread: the AInputQueue has changed.  Upon processing
         * this command, android_app->inputQueue will be updated to the new queue
         * (or NULL).
         */
        case APP_CMD_INPUT_CHANGED:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_INPUT_CHANGED\n");
            break;

        /**
         * Command from main thread: a new ANativeWindow is ready for use.  Upon
         * receiving this command, android_app->window will contain the new window
         * surface.
         */
        case APP_CMD_INIT_WINDOW:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_INIT_WINDOW %p\n", self);
            self->hasWindow = true;
            self->app->readyForInit();
            break;

        /**
         * Command from main thread: the existing ANativeWindow needs to be
         * terminated.  Upon receiving this command, android_app->window still
         * contains the existing window; after calling android_app_exec_cmd
         * it will be set to NULL.
         */
        case APP_CMD_TERM_WINDOW:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_TERM_WINDOW\n");
            self->hasWindow = false;
            self->app->requestSuspend();
            break;

        /**
         * Command from main thread: the current ANativeWindow has been resized.
         * Please redraw with its new size.
         */
        case APP_CMD_WINDOW_RESIZED:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_WINDOW_RESIZED\n");
            break;

        /**
         * Command from main thread: the system needs that the current ANativeWindow
         * be redrawn.  You should redraw the window before handing this to
         * android_app_exec_cmd() in order to avoid transient drawing glitches.
         */
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_WINDOW_REDRAW_NEEDED\n");
            break;

        /**
         * Command from main thread: the content area of the window has changed,
         * such as from the soft input window being shown or hidden.  You can
         * find the new content rect in android_app::contentRect.
         */
        case APP_CMD_CONTENT_RECT_CHANGED:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_CONTENT_RECT_CHANGED\n");
            break;

        /**
         * Command from main thread: the app's activity window has gained
         * input focus.
         */
        case APP_CMD_GAINED_FOCUS:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_GAINED_FOCUS\n");
            self->hasFocus = true;

            // enable sensor events
            if (self->accelSensor) {
                ASensorEventQueue_enableSensor(self->sensorEventQueue, self->accelSensor);
                ASensorEventQueue_setEventRate(self->sensorEventQueue, self->accelSensor, (1000L/60)*1000);
            }   
            if (self->gyroSensor) {
                ASensorEventQueue_enableSensor(self->sensorEventQueue, self->gyroSensor);
                ASensorEventQueue_setEventRate(self->sensorEventQueue, self->gyroSensor, (1000L/60)*1000);
            }         
            break;

        /**
         * Command from main thread: the app's activity window has lost
         * input focus.
         */
        case APP_CMD_LOST_FOCUS:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_LOST_FOCUS\n");
            self->hasFocus = false;

            // disable sensor events
            if (self->accelSensor) {
                ASensorEventQueue_disableSensor(self->sensorEventQueue, self->accelSensor);
            }
            if (self->gyroSensor) {
                ASensorEventQueue_disableSensor(self->sensorEventQueue, self->gyroSensor);
            }
            break;

        /**
         * Command from main thread: the current device configuration has changed.
         */
        case APP_CMD_CONFIG_CHANGED:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_CONFIG_CHANGED\n");
            break;

        /**
         * Command from main thread: the system is running low on memory.
         * Try to reduce your memory use.
         */
        case APP_CMD_LOW_MEMORY:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_LOW_MEMORY\n");
            break;

        /**
         * Command from main thread: the app's activity has been started.
         */
        case APP_CMD_START:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_START\n");
            break;

        /**
         * Command from main thread: the app's activity has been resumed.
         */
        case APP_CMD_RESUME:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_RESUME\n");
            break;

        /**
         * Command from main thread: the app should generate a new saved state
         * for itself, to restore from later if needed.  If you have saved state,
         * allocate it with malloc and place it in android_app.savedState with
         * the size in android_app.savedStateSize.  The will be freed for you
         * later.
         */
        case APP_CMD_SAVE_STATE:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_SAVE_STATE\n");
            break;

        /**
         * Command from main thread: the app's activity has been paused.
         */
        case APP_CMD_PAUSE:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_PAUSE\n");
            break;

        /**
         * Command from main thread: the app's activity has been stopped.
         */
        case APP_CMD_STOP:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_STOP\n");
            break;

        /**
         * Command from main thread: the app's activity is being destroyed,
         * and waiting for the app thread to clean up and exit before proceeding.
         */
        case APP_CMD_DESTROY:
            Log::Info("androidBridge::onAppCmd(): APP_CMD_DESTROY\n");
            break;

        default:
            Log::Warn("androidBridge::onAppCmd(): UNKNOWN!\n");
            break;
    }
}

} // namespace _priv
} // namespace Oryol
