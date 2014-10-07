//------------------------------------------------------------------------------
//  iosInputMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "iosInputMgr.h"
#include "Core/ios/iosBridge.h"
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

} // namespace _priv
} // namespace Oryol
