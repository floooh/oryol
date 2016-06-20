//------------------------------------------------------------------------------
//  inputMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputMgrBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
inputMgrBase::inputMgrBase() :
valid(false) {
    this->keyboard.dispatcher = &this->dispatcher;
    this->mouse.dispatcher = &this->dispatcher;
    this->touchpad.dispatcher = &this->dispatcher;
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
    if (this->keyboard.attached) {
        this->keyboard.reset();
    }
    if (this->mouse.attached) {
        this->mouse.reset();
    }
    if (this->touchpad.attached) {
        this->touchpad.reset();
    }
    for (int i = 0; i < MaxNumGamepads; i++) {
        if (this->gamepad[i].attached) {
            this->gamepad[i].reset();
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
inputMgrBase::onTouchEvent(const touchEvent& event) {
    o_assert_dbg(event.numTouches > 0);
    if (this->touchpad.attached) {
        if (this->inputSetup.TapEnabled) {
            if (gestureState::action == this->singleTapDetector.detect(event)) {
                this->touchpad.onTapped(this->singleTapDetector.position);
            }
        }
        if (this->inputSetup.DoubleTapEnabled) {
            if (gestureState::action == this->doubleTapDetector.detect(event)) {
                this->touchpad.onDoubleTapped(this->doubleTapDetector.position);
            }
        }
        if (this->inputSetup.PanEnabled) {
            switch (this->panDetector.detect(event)) {
                case gestureState::start:
                    this->touchpad.onPanningStarted(this->panDetector.position, this->panDetector.startPosition);
                    break;
                case gestureState::move:
                    this->touchpad.onPanning(this->panDetector.position, this->panDetector.startPosition);
                    break;
                case gestureState::end:
                    this->touchpad.onPanningEnded(this->panDetector.position, this->panDetector.startPosition);
                    break;
                default:
                    if (this->touchpad.panning) {
                        this->touchpad.onPanningCancelled();
                    }
                    break;
            }
        }
        if (this->inputSetup.PinchEnabled) {
            switch (this->pinchDetector.detect(event)) {
                case gestureState::start:
                    this->touchpad.onPinchingStarted(
                        this->pinchDetector.position0,
                        this->pinchDetector.position1,
                        this->pinchDetector.startPosition0,
                        this->pinchDetector.startPosition1);
                    break;
                case gestureState::move:
                    this->touchpad.onPinching(
                        this->pinchDetector.position0,
                        this->pinchDetector.position1,
                        this->pinchDetector.startPosition0,
                        this->pinchDetector.startPosition1);
                    break;
                case gestureState::end:
                    this->touchpad.onPinchingEnded(
                        this->pinchDetector.position0,
                        this->pinchDetector.position1,
                        this->pinchDetector.startPosition0,
                        this->pinchDetector.startPosition1);
                    break;
                default:
                    if (this->touchpad.pinching) {
                        this->touchpad.onPinchingCancelled();
                    }
                    break;
            }
        }
    }
}

} // namespace _priv
} // namespace Oryol

