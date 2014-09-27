//------------------------------------------------------------------------------
//  androidInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "androidInputMgr.h"
#include "Core/Core.h"
#include "android_native/android_native_app_glue.h"

// this is in the app's main file (see App.h -> OryolApp)
extern android_app* OryolAndroidAppState;

using namespace ndk_helper;

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
    self = nullptr;
}

//------------------------------------------------------------------------------
void
androidInputMgr::setup(const InputSetup& setup) {

    inputMgrBase::setup(setup);

    this->touchpad.Attached = true;
    if (setup.TapEnabled) {
        this->tapDetector.SetConfiguration(OryolAndroidAppState->config);
    }
    if (setup.DoubleTapEnabled) {
        this->doubleTapDetector.SetConfiguration(OryolAndroidAppState->config);
    }
    if (setup.PinchEnabled) {
        this->pinchDetector.SetConfiguration(OryolAndroidAppState->config);
    }
    if (setup.PanEnabled) {
        this->dragDetector.SetConfiguration(OryolAndroidAppState->config);
    }
    OryolAndroidAppState->onInputEvent = androidInputMgr::onInputEvent;
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });    
}

//------------------------------------------------------------------------------
void
androidInputMgr::discard() {

    OryolAndroidAppState->onInputEvent = nullptr;
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;

    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
androidInputMgr::reset() {
    if (this->keyboard.Attached) {
        this->keyboard.reset();
    }
    if (this->mouse.Attached) {
        this->mouse.reset();
    }
    if (this->touchpad.Attached) {
        this->touchpad.reset();
    }
    for (int32 i = 0; i < MaxNumGamepads; i++) {
        if (this->gamepads[i].Attached) {
            this->gamepads[i].reset();
        }
    }    
}

//------------------------------------------------------------------------------
int32_t
androidInputMgr::onInputEvent(struct android_app* app, AInputEvent* event) {
    o_assert_dbg(nullptr != self);

    int32_t retval = 0;
    const int32_t type = AInputEvent_getType(event);
    if (AINPUT_EVENT_TYPE_MOTION == type) {

        glm::vec2 pos;
        pos.x = AMotionEvent_getX(event, 0);
        pos.y = AMotionEvent_getY(event, 0);

        if (self->inputSetup.DoubleTapEnabled) {
            GESTURE_STATE doubleTapState = self->doubleTapDetector.Detect(event);
            if (doubleTapState & GESTURE_STATE_ACTION) {
                self->touchpad.DoubleTapped = true;
                self->touchpad.onPos(0, pos);
            }
        }

        if (self->inputSetup.TapEnabled) {
            GESTURE_STATE tapState = self->tapDetector.Detect(event);
            if (tapState & GESTURE_STATE_ACTION) {
                self->touchpad.Tapped = true;
                self->touchpad.onPos(0, pos);
            }
        }

        if (self->inputSetup.PanEnabled) {
            GESTURE_STATE dragState = self->dragDetector.Detect(event);
            if (dragState & GESTURE_STATE_START) {
                // need to init position
                Vec2 v;
                self->dragDetector.GetPointer(v);
                self->touchpad.onPos(0, glm::vec2(v.x_, v.y_));
            }
            if (dragState & GESTURE_STATE_MOVE) {
                Vec2 v;
                self->dragDetector.GetPointer(v);
                self->touchpad.Panning = true;
                self->touchpad.onPosMov(0, glm::vec2(v.x_, v.y_));
            }
        }

        if (self->inputSetup.PinchEnabled) {
            GESTURE_STATE pinchState = self->pinchDetector.Detect(event);
            if (pinchState & GESTURE_STATE_START) {
                // need to init positions
                Vec2 v0, v1;
                self->pinchDetector.GetPointers(v0, v1);
                self->touchpad.onPos(0, glm::vec2(v0.x_, v0.y_));
                self->touchpad.onPos(1, glm::vec2(v1.x_, v1.y_));
            }
            if (pinchState & GESTURE_STATE_MOVE) {
                Vec2 v0, v1;
                self->pinchDetector.GetPointers(v0, v1);
                self->touchpad.Pinching = true;
                self->touchpad.onPosMov(0, glm::vec2(v0.x_, v0.y_));
                self->touchpad.onPosMov(1, glm::vec2(v1.x_, v1.y_));
            }
        }
    }
    else if (AINPUT_EVENT_TYPE_KEY == type) {
        Log::Dbg("androidInputMgr: key event received (not handled)!\n");
    }
    else {
        Log::Dbg("androidInputMgr: unknown input event type '%d'\n", type);
    }
    return retval;
}

} // namespace _priv
} // namespace Oryol