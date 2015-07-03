//------------------------------------------------------------------------------
//  osxAppProxy.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxAppProxy.h"
#include "Core/Assertion.h"
#include "Core/App.h"
#import <Cocoa/Cocoa.h>

//------------------------------------------------------------------------------
//  our NSApplication subclass
//
@interface oryolApplication : NSApplication
@end

@implementation oryolApplication
// From http://cocoadev.com/index.pl?GameKeyboardHandlingAlmost
// This works around an AppKit bug, where key up events while holding
// down the command key don't get sent to the key window.
- (void)sendEvent:(NSEvent *)event {
    if ([event type] == NSKeyUp && ([event modifierFlags] & NSCommandKeyMask)) {
        [[self keyWindow] sendEvent:event];
    }
    else {
        [super sendEvent:event];
    }
}
@end

//------------------------------------------------------------------------------
//  our application delegate class
//
@interface oryolApplicationDelegate : NSObject
@end

@implementation oryolApplicationDelegate
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    // FIXME!
    return NSTerminateCancel;
}

- (void)applicationDidChangeScreenParameters:(NSNotification *) notification
{
    // FIXME!
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    [NSApp stop:nil];
}
@end

id oryolAutoReleasePool = nil;
id oryolAppDelegate = nil;

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
osxAppProxy::init() {
    o_assert(!NSApp);

    oryolAutoReleasePool = [[NSAutoreleasePool alloc] init];

    // Implicitly create shared NSApplication instance
    [oryolApplication sharedApplication];

    // In case we are unbundled, make us a proper UI application
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // There can only be one application delegate, but we allocate it the
    // first time a window is created to keep all window code in this file
    oryolAppDelegate = [[oryolApplicationDelegate alloc] init];
    o_assert(nil != oryolAppDelegate);

    [NSApp setDelegate:oryolAppDelegate];
    [NSApp run];
}

//------------------------------------------------------------------------------
void
osxAppProxy::terminate() {
    if (oryolAppDelegate) {
        [NSApp setDelegate:nil];
        oryolAppDelegate = nil;
    }
    [oryolAutoReleasePool release];
    oryolAutoReleasePool = nil;
}

//------------------------------------------------------------------------------
void
osxAppProxy::onFrame(App* app) {

    app->onFrame();

    // poll system events
    for (;;) {
        NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event == nil) {
            break;
        }
        [NSApp sendEvent:event];
    }

    [oryolAutoReleasePool drain];
    oryolAutoReleasePool = [[NSAutoreleasePool alloc] init];
}



} // namespace _priv
} // namespace Oryol