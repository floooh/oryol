//------------------------------------------------------------------------------
//  iosInputMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "iosInputMgr.h"
#include "Core/ios/iosBridge.h"
#include "Input/touch/touchEvent.h"
#include "Time/Clock.h"
#import <UIKit/UIKit.h>

using namespace Oryol::_priv;

static Oryol::_priv::iosInputMgr* iosInputMgrPtr = nullptr;

@interface iosInputDelegate : NSObject
@end

@implementation iosInputDelegate

- (void) onTouchEvent:(touchEvent::touchType)type withTouches:(NSSet*)touches withEvent:(UIEvent*)event {
    o_assert(iosInputMgrPtr);

    touchEvent newEvent;
    newEvent.type = type;
    newEvent.time = Oryol::Clock::Now();
    NSEnumerator* enumerator = [[event allTouches] objectEnumerator];
    UITouch* curTouch;
    while ((curTouch = [enumerator nextObject])) {
        if ((newEvent.numTouches + 1) < touchEvent::MaxNumPoints) {
            CGPoint pos = [curTouch locationInView:curTouch.view];
            touchEvent::point& curPoint = newEvent.points[newEvent.numTouches++];
            curPoint.identifier = (Oryol::uintptr) curTouch;
            curPoint.pos.x = pos.x;
            curPoint.pos.y = pos.y;
            curPoint.isChanged = [touches containsObject:curTouch];
        }
    }
    iosInputMgrPtr->onTouchEvent(newEvent);
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touchEvent::began withTouches:touches withEvent:event];
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touchEvent::moved withTouches:touches withEvent:event];
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touchEvent::ended withTouches:touches withEvent:event];
}

- (void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touchEvent::cancelled withTouches:touches withEvent:event];
}
@end

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
iosInputMgr::iosInputMgr() :
inputDelegate(nil)
{
    o_assert(nullptr == iosInputMgrPtr);
    iosInputMgrPtr = this;
}

//------------------------------------------------------------------------------
iosInputMgr::~iosInputMgr() {
    o_assert(this == iosInputMgrPtr);
    iosInputMgrPtr = nullptr;
}

//------------------------------------------------------------------------------
void
iosInputMgr::setup(const InputSetup& setup) {
    
    inputMgrBase::setup(setup);
    this->touchpad.Attached = true;
    this->singleTapDetector.numRequiredTaps = 1;
    this->doubleTapDetector.numRequiredTaps = 2;
    
    // create the input delegate object
    this->inputDelegate = [[iosInputDelegate alloc] init];
    
    // set delegate in our overriden GLKView
    oryolGLKView* glkView = iosBridge::ptr()->iosGetGLKView();
    [glkView setTouchDelegate:this->inputDelegate];
    
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
iosInputMgr::discard() {
    
    // detach from runloop
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;

    // remove touch delegate
    oryolGLKView* glkView = iosBridge::ptr()->iosGetGLKView();
    [glkView setTouchDelegate:nil];
    
    [this->inputDelegate release];
    this->inputDelegate = nil;
    
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
iosInputMgr::reset() {
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
void
iosInputMgr::onTouchEvent(const Oryol::_priv::touchEvent &event) {
    o_assert_dbg(this->isValid());
    
    // feed event into gestures detectors and check for detected gestures
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
