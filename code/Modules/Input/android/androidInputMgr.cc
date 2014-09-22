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
androidInputMgr::androidInputMgr() {
    o_assert_dbg(nullptr == self);
    this->touchpad.Attached = true;
    self = this;
    this->tapDetector.SetConfiguration(OryolAndroidAppState->config);
    this->doubleTapDetector.SetConfiguration(OryolAndroidAppState->config);
    this->pinchDetector.SetConfiguration(OryolAndroidAppState->config);
    this->dragDetector.SetConfiguration(OryolAndroidAppState->config);
    OryolAndroidAppState->onInputEvent = androidInputMgr::onInputEvent;
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
androidInputMgr::~androidInputMgr() {
    self = nullptr;
    OryolAndroidAppState->onInputEvent = nullptr;
    Core::PostRunLoop()->Remove(this->runLoopId);
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

        GESTURE_STATE tapState = self->tapDetector.Detect(event);
        GESTURE_STATE doubleTapState = self->doubleTapDetector.Detect(event);
        GESTURE_STATE pinchState = self->pinchDetector.Detect(event);
        GESTURE_STATE dragState = self->dragDetector.Detect(event);
        glm::vec2 pos;
        pos.x = AMotionEvent_getX(event, 0);
        pos.y = AMotionEvent_getY(event, 0);

        if (doubleTapState & GESTURE_STATE_ACTION) {
            self->touchpad.DoubleTapped = true;
            self->touchpad.onPos(0, pos);
        }
        if (tapState & GESTURE_STATE_ACTION) {
            self->touchpad.Tapped = true;
            self->touchpad.onPos(0, pos);
        }
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