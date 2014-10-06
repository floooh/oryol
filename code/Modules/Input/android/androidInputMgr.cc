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

    inputMgrBase::setup(setup);

    this->touchpad.Attached = true;
    this->singleTapDetector.numRequiredTaps = 1;
    this->doubleTapDetector.numRequiredTaps = 2;

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
                event.type = touchEvent::invalid;
                break;
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
        if (touchEvent::invalid != event.type) {
            self->handleTouchEvent(event);
            retval = 1;
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

//------------------------------------------------------------------------------
void
androidInputMgr::handleTouchEvent(const touchEvent& event) {

    // feed gesture detectors
    if (this->inputSetup.TapEnabled) {
        if (gestureState::action == this->singleTapDetector.detect(event)) {
            this->touchpad.Tapped = true;
            this->touchpad.onPos(0, this->singleTapDetector.position);
            this->touchpad.onStartPos(0, this->singleTapDetector.position);
        }
    }
    if (this->inputSetup.DoubleTapEnabled) {
        if (gestureState::action == this->doubleTapDetector.detect(event)) {
            this->touchpad.DoubleTapped = true;
            this->touchpad.onPos(0, this->doubleTapDetector.position);
            this->touchpad.onStartPos(0, this->doubleTapDetector.position);
        }
    }
    if (this->inputSetup.PanEnabled) {
        switch (this->panDetector.detect(event)) {
            case gestureState::start:
                this->touchpad.PanningStarted = true;
                this->touchpad.Panning = true;
                this->touchpad.onPos(0, this->panDetector.position);
                this->touchpad.onStartPos(0, this->panDetector.startPosition);
                break;
            case gestureState::move:
                this->touchpad.Panning = true;
                this->touchpad.onPosMov(0, this->panDetector.position);
                this->touchpad.onStartPos(0, this->panDetector.startPosition);
                break;
            case gestureState::end:
                this->touchpad.PanningEnded = true;
                this->touchpad.Panning = false;
                this->touchpad.onPos(0, this->panDetector.position);
                this->touchpad.onStartPos(0, this->panDetector.startPosition);
                break;
            default:
                this->touchpad.Panning = false;
                break;
        }
    }
    if (this->inputSetup.PinchEnabled) {
        switch (this->pinchDetector.detect(event)) {
            case gestureState::start:
                this->touchpad.PinchingStarted = true;
                this->touchpad.Pinching = true;
                this->touchpad.onPos(0, this->pinchDetector.position0);
                this->touchpad.onPos(1, this->pinchDetector.position1);
                this->touchpad.onStartPos(0, this->pinchDetector.startPosition0);
                this->touchpad.onStartPos(1, this->pinchDetector.startPosition1);
                break;
            case gestureState::move:
                this->touchpad.Pinching = true;
                this->touchpad.onPosMov(0, this->pinchDetector.position0);
                this->touchpad.onPosMov(1, this->pinchDetector.position1);
                this->touchpad.onStartPos(0, this->pinchDetector.startPosition0);
                this->touchpad.onStartPos(1, this->pinchDetector.startPosition1);
                break;
            case gestureState::end:
                this->touchpad.PinchingEnded = true;
                this->touchpad.Pinching = false;
                this->touchpad.onPos(0, this->pinchDetector.position0);
                this->touchpad.onPos(1, this->pinchDetector.position1);
                this->touchpad.onStartPos(0, this->pinchDetector.startPosition0);
                this->touchpad.onStartPos(1, this->pinchDetector.startPosition1);
                break;
            default:
                this->touchpad.Pinching = false;
                break;
        }
    }
}

} // namespace _priv
} // namespace Oryol