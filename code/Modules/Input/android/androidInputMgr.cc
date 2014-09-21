//------------------------------------------------------------------------------
//  androidInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "androidInputMgr.h"
#include "Core/Core.h"
#include "android_native/android_native_app_glue.h"

// this is in the app's main file (see App.h -> OryolApp)
extern android_app* OryolAndroidAppState;

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
androidInputMgr::androidInputMgr() {
    OryolAndroidAppState->onInputEvent = androidInputMgr::onInputEvent;    
    this->runLoopId = Core::RunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
androidInputMgr::~androidInputMgr() {
    OryolAndroidAppState->onInputEvent = nullptr;
    Core::RunLoop()->Remove(this->runLoopId);
}

//------------------------------------------------------------------------------
void
androidInputMgr::reset() {
    if (this->keyboard.Attached()) {
        this->keyboard.reset();
    }
    if (this->mouse.Attached()) {
        this->mouse.reset();
    }
    if (this->touchpad.Attached()) {
        this->touchpad.reset();
    }
    for (int32 i = 0; i < MaxNumGamepads; i++) {
        if (this->gamepads[i].Attached()) {
            this->gamepads[i].reset();
        }
    }    
}

//------------------------------------------------------------------------------
int32_t
androidInputMgr::onInputEvent(struct android_app* app, AInputEvent* event) {
    int32_t retval = 0;
    const int32_t type = AInputEvent_getType(event);

    const char* source = "unknown";
    switch (AInputEvent_getSource(event)) {
        case AINPUT_SOURCE_KEYBOARD:    source = "keyboard"; break;
        case AINPUT_SOURCE_DPAD:        source = "dpad"; break;
        case AINPUT_SOURCE_GAMEPAD:     source = "gamepad"; break;
        case AINPUT_SOURCE_TOUCHSCREEN: source = "touchscreen"; break;
        case AINPUT_SOURCE_MOUSE:       source = "mouse"; break;
        case AINPUT_SOURCE_STYLUS:      source = "stylus"; break;
        case AINPUT_SOURCE_TRACKBALL:   source = "trackball"; break;
        case AINPUT_SOURCE_TOUCHPAD:    source = "touchpad"; break;
        case AINPUT_SOURCE_TOUCH_NAVIGATION: source = "touchnavigation"; break;
        case AINPUT_SOURCE_JOYSTICK:    source = "joystick"; break;
    }

    if (AINPUT_EVENT_TYPE_MOTION == type) {
        const int32_t actionAll = AMotionEvent_getAction(event);
        const int32_t pointerIndex = (actionAll & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        const int32_t action = actionAll & AMOTION_EVENT_ACTION_MASK;

        const char* actionStr = "unknown";
        switch (action) {
            case AMOTION_EVENT_ACTION_DOWN:         actionStr = "AMOTION_EVENT_ACTION_DOWN"; break;
            case AMOTION_EVENT_ACTION_UP:           actionStr = "AMOTION_EVENT_ACTION_UP"; break;
            case AMOTION_EVENT_ACTION_MOVE:         actionStr = "AMOTION_EVENT_ACTION_MOVE"; break;
            case AMOTION_EVENT_ACTION_CANCEL:       actionStr = "AMOTION_EVENT_ACTION_CANCEL"; break;
            case AMOTION_EVENT_ACTION_OUTSIDE:      actionStr = "AMOTION_EVENT_ACTION_OUTSIDE"; break;
            case AMOTION_EVENT_ACTION_POINTER_DOWN: actionStr = "AMOTION_EVENT_ACTION_POINTER_DOWN"; break;
            case AMOTION_EVENT_ACTION_POINTER_UP:   actionStr = "AMOTION_EVENT_ACTION_POINTER_UP"; break;
            case AMOTION_EVENT_ACTION_HOVER_MOVE:   actionStr = "AMOTION_EVENT_ACTION_HOVER_MOVE"; break;
            case AMOTION_EVENT_ACTION_SCROLL:       actionStr = "AMOTION_EVENT_ACTION_SCROLL"; break;
            case AMOTION_EVENT_ACTION_HOVER_ENTER:  actionStr = "AMOTION_EVENT_ACTION_HOVER_ENTER"; break;
            case AMOTION_EVENT_ACTION_HOVER_EXIT:   actionStr = "AMOTION_EVENT_ACTION_HOVER_EXIT"; break;
        }
        Log::Dbg("androidInputMgr: src=%s, action=%s, pointer=%d\n", source, actionStr, pointerIndex);
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