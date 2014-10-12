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
    if (setup.AccelerometerEnabled || setup.GyrometerEnabled) {
        this->motionManager = [[CMMotionManager alloc] init];
        if ([this->motionManager isDeviceMotionAvailable]) {
            [this->motionManager startDeviceMotionUpdates];
            this->sensors.Attached = true;
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
            [this->motionManager stopAccelerometerUpdates];
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
    CMDeviceMotion* motionData = [this->motionManager deviceMotion];
    if (nil != motionData) {
        CMAcceleration cmGravity = motionData.gravity;
        CMAcceleration cmUserAccel = motionData.userAcceleration;
        
        // acceleration
        if (this->inputSetup.AccelerometerEnabled) {
            static const float32 earthGravity = 9.80665;
            glm::vec3 accel(cmGravity.x + cmUserAccel.x, cmGravity.y + cmUserAccel.y, cmGravity.z + cmUserAccel.z);
            this->sensors.Acceleration = accel * earthGravity;
        }
        
        // attitude
        if (this->inputSetup.GyrometerEnabled) {
            this->sensors.Yaw   = motionData.attitude.yaw;
            this->sensors.Pitch = motionData.attitude.pitch;
            this->sensors.Roll  = motionData.attitude.roll;
        }
    }
}

} // namespace _priv
} // namespace Oryol
