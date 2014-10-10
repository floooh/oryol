//------------------------------------------------------------------------------
//  iosInputMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "iosInputMgr.h"
#include "Core/ios/iosBridge.h"
#include "Time/Clock.h"
#import <UIKit/UIKit.h>
#import <CoreMotion/CoreMotion.h>

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
inputDelegate(nil),
motionManager(nil),
resetRunLoopId(RunLoop::InvalidId),
motionRunLoopId(RunLoop::InvalidId)
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
    
    // create CoreMotionManager to sample device motion data
    if (setup.AccelerometerEnabled) {
        this->motionManager = [[CMMotionManager alloc] init];
        if ([this->motionManager isDeviceMotionAvailable]) {
            [this->motionManager startDeviceMotionUpdates];
            this->accelerometer.Attached = true;
            this->motionRunLoopId = Core::PreRunLoop()->Add([this]() { this->sampleMotionData(); });
        }
        else {
            this->motionRunLoopId = RunLoop::InvalidId;
        }
    }
    
    // set delegate in our overriden GLKView
    oryolGLKView* glkView = iosBridge::ptr()->iosGetGLKView();
    [glkView setTouchDelegate:this->inputDelegate];
    
    // add reset callback to post-runloop
    this->resetRunLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
iosInputMgr::discard() {
    
    // detach from runloops
    Core::PostRunLoop()->Remove(this->resetRunLoopId);
    this->resetRunLoopId = RunLoop::InvalidId;

    // remove touch delegate
    oryolGLKView* glkView = iosBridge::ptr()->iosGetGLKView();
    [glkView setTouchDelegate:nil];
    
    if (nil != this->motionManager) {
        if (RunLoop::InvalidId != this->motionRunLoopId) {
            Core::PreRunLoop()->Remove(this->motionRunLoopId);
            this->motionRunLoopId = RunLoop::InvalidId;
            [this->motionManager stopDeviceMotionUpdates];
        }
        [this->motionManager release];
        this->motionManager = nil;
    }
    [this->inputDelegate release];
    this->inputDelegate = nil;
    
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
iosInputMgr::sampleMotionData() {
    o_assert_dbg(nil != this->motionManager);
    CMDeviceMotion* motion = [this->motionManager deviceMotion];
    if (nil != motion) {
        CMAcceleration gravity = motion.gravity;
        CMAcceleration userAccel = motion.userAcceleration;
        static const float32 earthGravity = 9.80665f;
        
        // note: flip x and y, since we're by default in landscape orientation
        glm::vec3 accel(-(userAccel.y + gravity.y), userAccel.x + gravity.x, userAccel.z + gravity.z);
        accel *= earthGravity;
        this->accelerometer.Acceleration = accel;
    }
}

} // namespace _priv
} // namespace Oryol
