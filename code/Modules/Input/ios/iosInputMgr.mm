//------------------------------------------------------------------------------
//  iosInputMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "iosInputMgr.h"
#include "Core/ios/iosBridge.h"
#include "Input/touch/touch.h"
#include "Time/Clock.h"
#import <UIKit/UIKit.h>

using namespace Oryol::_priv;

static Oryol::_priv::iosInputMgr* iosInputMgrPtr = nullptr;

@interface iosInputDelegate : NSObject
@end

@implementation iosInputDelegate

- (void) onTouchEvent:(touch::touchType)type withTouches:(NSSet*)touches withEvent:(UIEvent*)event {
    o_assert(iosInputMgrPtr);

    touch touchEvent;
    touchEvent.type = type;
    touchEvent.time = Oryol::Clock::Now();
    NSEnumerator* enumerator = [[event allTouches] objectEnumerator];
    UITouch* curTouch;
    while ((curTouch = [enumerator nextObject])) {
        if ((touchEvent.numTouches + 1) < touch::MaxNumPoints) {
            CGPoint pos = [curTouch locationInView:curTouch.view];
            touch::point& curPoint = touchEvent.points[touchEvent.numTouches++];
            curPoint.identifier = (Oryol::uintptr) curTouch;
            curPoint.x = pos.x;
            curPoint.y = pos.y;
            curPoint.isChanged = [touches containsObject:curTouch];
        }
    }
    iosInputMgrPtr->onTouchEvent(touchEvent);
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touch::began withTouches:touches withEvent:event];
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touch::moved withTouches:touches withEvent:event];
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touch::ended withTouches:touches withEvent:event];
}

- (void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event {
    [self onTouchEvent:touch::cancelled withTouches:touches withEvent:event];
}
@end

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
iosInputMgr::iosInputMgr() :
screenWidth(0.0f),
screenHeight(0.0f),
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
    
    // get current screen size
    CGRect mainScreenBounds = [[UIScreen mainScreen] bounds];
    this->screenWidth = mainScreenBounds.size.width;
    this->screenHeight = mainScreenBounds.size.height;
    
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
glm::vec2
iosInputMgr::toLandscape(float32 x, float32 y) {
    return glm::vec2(y, this->screenWidth - x);
}

//------------------------------------------------------------------------------
void
iosInputMgr::onTouchEvent(const Oryol::_priv::touch &touchEvent) {
    o_assert_dbg(this->isValid());
    
    // FIXME: DEBUG OUTPUT
    const char* type;
    switch (touchEvent.type) {
        case touch::began: type = "began"; break;
        case touch::moved: type = "moved"; break;
        case touch::ended: type = "ended"; break;
        default: type = "invalid"; break;
    }
    Log::Info("touchEvent: type=%s, time=%f, numTouches=%d\n", type, touchEvent.time.Since(0).AsSeconds(), touchEvent.numTouches);
    for (int i = 0; i < touchEvent.numTouches; i++) {
        Log::Info("  point %d: id=%ld, x=%.2f, y=%.2f, changed=%s\n",
            i,
            touchEvent.points[i].identifier,
            touchEvent.points[i].x,
            touchEvent.points[i].y,
            touchEvent.points[i].isChanged ? "yes" : "no");
    }
    
    // feed event into gestures detectors and check for detected gestures
    if (gestureState::action == this->tapDetector.detect(touchEvent)) {
        this->touchpad.Tapped = true;
        this->touchpad.onPos(0, this->tapDetector.pos());
    }    
}

} // namespace _priv
} // namespace Oryol
