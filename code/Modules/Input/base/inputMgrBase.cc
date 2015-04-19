//------------------------------------------------------------------------------
//  inputMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputMgrBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
inputMgrBase::inputMgrBase() :
valid(false),
cursorMode(CursorMode::InvalidCursorMode) {
    // empty
}
    
//------------------------------------------------------------------------------
inputMgrBase::~inputMgrBase() {
    o_assert_dbg(!this->isValid());
}

//------------------------------------------------------------------------------
void
inputMgrBase::setup(const InputSetup& setup) {
    o_assert_dbg(!this->isValid());
    this->singleTapDetector.numRequiredTaps = 1;
    this->doubleTapDetector.numRequiredTaps = 2;        
    this->valid = true;
    this->inputSetup = setup;
    this->cursorMode = CursorMode::Normal;
}

//------------------------------------------------------------------------------
void
inputMgrBase::discard() {
    o_assert_dbg(this->isValid());
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
inputMgrBase::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
inputMgrBase::attachInputHandler(const Ptr<Port>& handler) {
    o_assert(InvalidIndex == this->handlers.FindIndexLinear(handler));
    this->handlers.Add(handler);
}

//------------------------------------------------------------------------------
void
inputMgrBase::detachInputHandler(const Ptr<Port>& handler) {
    int32 index = this->handlers.FindIndexLinear(handler);
    if (InvalidIndex != index) {
        this->handlers.Erase(index);
    }
}

//------------------------------------------------------------------------------
void
inputMgrBase::notifyHandlers(const Ptr<Message>& msg) {
    for (const auto& handler : this->handlers) {
        handler->Put(msg);
    }
}

//------------------------------------------------------------------------------
void
inputMgrBase::reset() {
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
const InputSetup&
inputMgrBase::getInputSetup() const {
    return this->inputSetup;
}

//------------------------------------------------------------------------------
void
inputMgrBase::beginCaptureText() {
    o_assert_dbg(this->valid);
    this->keyboard.beginCaptureText();
}

//------------------------------------------------------------------------------
void
inputMgrBase::endCaptureText() {
    o_assert_dbg(this->valid);
    this->keyboard.endCaptureText();
}

//------------------------------------------------------------------------------
void
inputMgrBase::setCursorMode(CursorMode::Code mode) {
    o_assert_range_dbg(mode, CursorMode::NumCursorModes);
    this->cursorMode = mode;
}

//------------------------------------------------------------------------------
void
inputMgrBase::onTouchEvent(const touchEvent& event) {

    if (this->touchpad.Attached) {
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
}

} // namespace _priv
} // namespace Oryol