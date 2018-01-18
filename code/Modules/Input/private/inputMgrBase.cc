//------------------------------------------------------------------------------
//  inputMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "inputMgrBase.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
inputMgrBase::inputMgrBase() {
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
    for (const auto& item : setup.GamepadMappings) {
        this->addGamepadMapping(item.Key(), item.Value());
    }
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
    for (int i = 0; i < inputDefs::maxNumGamepads; i++) {
        if (this->gamepad[i].attached) {
            this->gamepad[i].reset();
        }
    }    
}

//------------------------------------------------------------------------------
void
inputMgrBase::onTouchEvent(const touchEvent& event) {
    o_assert_dbg(event.numTouches > 0);
    if (this->touchpad.attached) {
        // track raw touch events
        if (event.numTouches == 1) {
            this->touchpad.onTouchEvent(event.type, event.points[0].pos);
        }
        // panning detection
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
        // tapping detection
        if (this->inputSetup.TapEnabled) {
            if (gestureState::action == this->singleTapDetector.detect(event)) {
                this->touchpad.onTapped(this->singleTapDetector.position);
            }
        }
        // double tap detection
        if (this->inputSetup.DoubleTapEnabled) {
            if (gestureState::action == this->doubleTapDetector.detect(event)) {
                this->touchpad.onDoubleTapped(this->doubleTapDetector.position);
            }
        }
        // pinc detection
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

//------------------------------------------------------------------------------
void
inputMgrBase::addGamepadMapping(const StringAtom& id, const GamepadMapping& mapping) {
    if (id == "__default") {
        this->defaultGamepadMapping = mapping;
    }
    else if (this->gamepadMappings.Contains(id)) {
        this->gamepadMappings[id] = mapping;
    }
    else {
        this->gamepadMappings.Add(id, mapping);
    }
}

//------------------------------------------------------------------------------
const GamepadMapping&
inputMgrBase::lookupGamepadMapping(const StringAtom& id) const {
    if (this->gamepadMappings.Contains(id)) {
        return this->gamepadMappings[id];
    }
    else {
        return this->defaultGamepadMapping;
    }
}

} // namespace _priv
} // namespace Oryol

