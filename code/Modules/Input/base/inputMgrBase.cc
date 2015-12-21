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
inputMgrBase::reset() {
    if (this->Keyboard.Attached) {
        this->Keyboard.reset();
    }
    if (this->Mouse.Attached) {
        this->Mouse.reset();
    }
    if (this->Touchpad.Attached) {
        this->Touchpad.reset();
    }
    for (int32 i = 0; i < MaxNumGamepads; i++) {
        if (this->Gamepad[i].Attached) {
            this->Gamepad[i].reset();
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
    this->Keyboard.beginCaptureText();
}

//------------------------------------------------------------------------------
void
inputMgrBase::endCaptureText() {
    o_assert_dbg(this->valid);
    this->Keyboard.endCaptureText();
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
    o_assert_dbg(event.numTouches > 0);
    if (this->Touchpad.Attached) {
        if (this->inputSetup.TapEnabled) {
            if (gestureState::action == this->singleTapDetector.detect(event)) {
                this->Touchpad.onTapped(this->singleTapDetector.position);
            }
        }
        if (this->inputSetup.DoubleTapEnabled) {
            if (gestureState::action == this->doubleTapDetector.detect(event)) {
                this->Touchpad.onDoubleTapped(this->doubleTapDetector.position);
            }
        }
        if (this->inputSetup.PanEnabled) {
            switch (this->panDetector.detect(event)) {
                case gestureState::start:
                    this->Touchpad.onPanningStarted(this->panDetector.position, this->panDetector.startPosition);
                    break;
                case gestureState::move:
                    this->Touchpad.onPanning(this->panDetector.position, this->panDetector.startPosition);
                    break;
                case gestureState::end:
                    this->Touchpad.onPanningEnded(this->panDetector.position, this->panDetector.startPosition);
                    break;
                default:
                    if (this->Touchpad.Panning) {
                        this->Touchpad.onPanningCancelled();
                    }
                    break;
            }
        }
        if (this->inputSetup.PinchEnabled) {
            switch (this->pinchDetector.detect(event)) {
                case gestureState::start:
                    this->Touchpad.onPinchingStarted(
                        this->pinchDetector.position0,
                        this->pinchDetector.position1,
                        this->pinchDetector.startPosition0,
                        this->pinchDetector.startPosition1);
                    break;
                case gestureState::move:
                    this->Touchpad.onPinching(
                        this->pinchDetector.position0,
                        this->pinchDetector.position1,
                        this->pinchDetector.startPosition0,
                        this->pinchDetector.startPosition1);
                    break;
                case gestureState::end:
                    this->Touchpad.onPinchingEnded(
                        this->pinchDetector.position0,
                        this->pinchDetector.position1,
                        this->pinchDetector.startPosition0,
                        this->pinchDetector.startPosition1);
                    break;
                default:
                    if (this->Touchpad.Pinching) {
                        this->Touchpad.onPinchingCancelled();
                    }
                    break;
            }
        }
    }
}

} // namespace _priv
} // namespace Oryol

