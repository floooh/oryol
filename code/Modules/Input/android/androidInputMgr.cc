//------------------------------------------------------------------------------
//  androidInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "androidInputMgr.h"
#include "Core/Core.h"
#include "Time/Clock.h"
#include "android_native/android_native_app_glue.h"

// this is in the app's main file (see App.h -> OryolApp)
extern android_app* OryolAndroidAppState;

namespace Oryol {
namespace _priv {

static androidInputMgr* self = nullptr;

//------------------------------------------------------------------------------
androidInputMgr::androidInputMgr() :
runLoopId(RunLoop::InvalidId) {
    o_assert_dbg(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
androidInputMgr::~androidInputMgr() {
    o_assert_dbg(this == self);
    self = nullptr;
}

//------------------------------------------------------------------------------
void
androidInputMgr::setup(const InputSetup& setup) {
    Log::Info("androidInputMgr::setup called!\n");

    inputMgrBase::setup(setup);
    this->touchpad.Attached = true;
    this->sensors.Attached = true;
    OryolAndroidAppState->onInputEvent = androidInputMgr::onInputEvent;
    androidBridge::ptr()->setSensorEventCallback(this->onSensorEvent);
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });   
}

//------------------------------------------------------------------------------
void
androidInputMgr::discard() {    
    Log::Info("androidInputMgr::discard called!\n");

    OryolAndroidAppState->onInputEvent = nullptr;
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
int32_t
androidInputMgr::onInputEvent(struct android_app* app, AInputEvent* aEvent) {
    o_assert_dbg(nullptr != self);

    int32_t retval = 0;
    const int32_t type = AInputEvent_getType(aEvent);
    if (AINPUT_EVENT_TYPE_MOTION == type) {

        int32_t action = AMotionEvent_getAction(aEvent);
        uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;        
        int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        // convert to touchEvent object
        touchEvent event;
        switch (flags) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                event.type = touchEvent::began;
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                event.type = touchEvent::ended;
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                event.type = touchEvent::moved;
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
                event.type = touchEvent::cancelled;
                break;
            default:
                return 0;
        }
        event.time = Oryol::Clock::Now();
        event.numTouches = AMotionEvent_getPointerCount(aEvent);
        for (int32 i = 0; i < event.numTouches; i++) {
            touchEvent::point& curPoint = event.points[i];
            curPoint.identifier = AMotionEvent_getPointerId(aEvent, i);
            curPoint.pos.x = AMotionEvent_getX(aEvent, i);
            curPoint.pos.y = AMotionEvent_getY(aEvent, i);
            curPoint.isChanged = (i == pointerIndex);
        }

        self->onTouchEvent(event);
        retval = 1;
    }
    else if (AINPUT_EVENT_TYPE_KEY == type) {
        Log::Dbg("androidInputMgr: key event received (not handled)!\n");
    }
    else {
        Log::Dbg("androidInputMgr: unknown input event type '%d'\n", type);
    }
    return retval;
}

//------------------------------------------------------------------------------
void
androidInputMgr::onSensorEvent(const ASensorEvent* event) {
    o_assert_dbg(nullptr != self);

    switch (event->type) {
        case ASENSOR_TYPE_ACCELEROMETER:
            if (self->inputSetup.AccelerometerEnabled) {
                // NOTE: x and y are swapped because the default orientation is landscape
                self->sensors.Acceleration.x = event->acceleration.y;
                self->sensors.Acceleration.y = -event->acceleration.x;
                self->sensors.Acceleration.z = -event->acceleration.z;
            }
            break;

        case ASENSOR_TYPE_GYROSCOPE:
            if (self->inputSetup.GyrometerEnabled) {
                // NOTE: the gyroscope values are *changes*
                // in orientation, not orientation :/
                // FIXME: compute absolute vector (look at browser 
                // source code to see how?)
                self->sensors.Yaw   += event->vector.roll * (1.0/60.0);
                self->sensors.Pitch += event->vector.azimuth * (1.0/60.0);
                self->sensors.Roll  += event->vector.pitch * (1.0/60.0);
            }
            break;

        default:
            break;
    }
}

} // namespace _priv
} // namespace Oryol