//------------------------------------------------------------------------------
//  osxAppBridge.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxAppBridge.h"
#include "Core/Assertion.h"
#include "Core/App.h"

using namespace Oryol;
using namespace Oryol::_priv;

//------------------------------------------------------------------------------
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
@interface oryolAppDelegate<NSApplicationDelegate> : NSObject
@end

@implementation oryolAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onDidFinishLaunching();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return NO;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender {
    osxAppBridge::ptr()->onShouldTerminate();
    return NSTerminateNow;
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onWillTerminate();
}

- (void)applicationWillBecomeActive:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onWillBecomeActive();
}

- (void)applicationDidBecomeActive:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onDidBecomeActive();
}

- (void)applicationWillResignActive:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onWillResignActive();
}

- (void)applicationDidResignActive:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onDidResignActive();
}

- (void)applicationWillHide:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onWillHide();
}

- (void)applicationDidHide:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onDidHide();
}

- (void)applicationWillUnhide:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onWillUnhide();
}

- (void)applicationDidUnhide:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onDidUnhide();
}

- (void)applicationDidChangeScreenParameters:(NSNotification*)aNotifaction {
    osxAppBridge::ptr()->onDidChangeScreenParameters();
}

- (void)applicationDidChangeOcclusionState:(NSNotification*)aNotification {
    osxAppBridge::ptr()->onDidChangeOcclusionState();
}
@end

//------------------------------------------------------------------------------
@interface oryolWindowDelegate<MTKViewDelegate> : NSObject
@end

@implementation oryolWindowDelegate
- (BOOL)windowShouldClose:(id)sender {
    osxAppBridge::ptr()->onWindowShouldClose();
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification {
    osxAppBridge::ptr()->onWindowDidResize();
}

- (void)windowDidMove:(NSNotification *)notification {
    osxAppBridge::ptr()->onWindowDidMove();
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    osxAppBridge::ptr()->onWindowDidMiniaturize();
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    osxAppBridge::ptr()->onWindowDidDeminiaturize();
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    osxAppBridge::ptr()->onWindowDidBecomeKey();
}

- (void)windowDidResignKey:(NSNotification *)notification {
    osxAppBridge::ptr()->onWindowDidResignKey();
}
@end

//------------------------------------------------------------------------------
@interface oryolViewDelegate<MTKViewDelegate> : NSObject
@end

@implementation oryolViewDelegate
- (void)view:(MTKView*)view willLayoutWithSize:(CGSize)size {
    // FIXME!
}

- (void)drawInView:(MTKView*)view
{
    @autoreleasepool {
        osxAppBridge::ptr()->onFrame();
    }
}
@end

namespace Oryol {
namespace _priv {

osxAppBridge* osxAppBridge::self = nullptr;

//------------------------------------------------------------------------------
osxAppBridge::osxAppBridge() :
app(nullptr),
shouldCloseFlag(false),
appDelegate(nil),
appWindow(nil),
appWindowDelegate(nil),
mtlDevice(nil),
mtkViewDelegate(nil),
mtkView(nil) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
osxAppBridge::~osxAppBridge() {
    o_assert(nil == this->app);
    o_assert(self);
    self = nullptr;
}

//------------------------------------------------------------------------------
osxAppBridge*
osxAppBridge::ptr() {
    o_assert_dbg(self);
    return self;
}

//------------------------------------------------------------------------------
void
osxAppBridge::setup(App* app_) {
    o_assert_dbg(nullptr == this->app);
    o_assert_dbg(nullptr != app_);
    Log::Info("osxAppBridge::setup() called\n");

    this->app = app_;
}

//------------------------------------------------------------------------------
void
osxAppBridge::discard() {
    o_assert_dbg(nullptr != this->app);
    Log::Info("osxAppBridge::discard() called\n");

    this->appWindow = nil;
    this->app = nullptr;
}

//------------------------------------------------------------------------------
bool
osxAppBridge::shouldClose() {
    return this->shouldCloseFlag;
}

//------------------------------------------------------------------------------
void
osxAppBridge::startMainLoop() {
    Log::Info("osxAppBridge::startMainLoop() called\n");
    [oryolApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    this->appDelegate = [[oryolAppDelegate alloc] init];
    [NSApp setDelegate:this->appDelegate];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDestroy() {
    [this->mtkView setDelegate:nil];
    [this->appWindow setDelegate:nil];
    [this->appWindow orderOut:nil];
    [NSApp stop:nil];
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidFinishLaunching() {
    Log::Info("osxAppBridge::onDidFinishLaunched() called\n");
    this->createWindow();
}

//------------------------------------------------------------------------------
void
osxAppBridge::onShouldTerminate() {
    Log::Info("osxAppBridge::onShouldTerminate() called\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWillTerminate() {
    Log::Info("osxAppBridge::onWillTerminate() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWillBecomeActive() {
    Log::Info("osxAppBridge::onWillBecomeActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidBecomeActive() {
    Log::Info("osxAppBridge::onDidBecomeActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWillResignActive() {
    Log::Info("osxAppBridge::onWillResignActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidResignActive() {
    Log::Info("osxAppBridge::onDidResignActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWillHide() {
    Log::Info("osxAppBridge::onWillHide() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidHide() {
    Log::Info("osxAppBridge::onDidHide() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWillUnhide() {
    Log::Info("osxAppBridge::onWillUnhide() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidUnhide() {
    Log::Info("osxAppBridge::onDidUnhide() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidChangeScreenParameters() {
    Log::Info("osxAppBridge::onDidChangeScreenParameters() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onDidChangeOcclusionState() {
    Log::Info("osxAppBridge::onDidChangeOcclusionState() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowShouldClose() {
    this->shouldCloseFlag = true;
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowDidResize() {
    Log::Info("osxAppBridge::onWindowDidResize() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowDidMove() {
    Log::Info("osxAppBridge::onWindowDidMove() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowDidMiniaturize() {
    Log::Info("osxAppBridge::onWindowDidMiniaturize() called\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowDidDeminiaturize() {
    Log::Info("osxAppBridge::osxWindowDidDeminiaturize() called\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowDidBecomeKey() {
    Log::Info("osxAppBridge::osxWindowDidBecomeKey() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onWindowDidResignKey() {
    Log::Info("osxAppBridge::osxWindowDidResignKey() called!\n");
}

//------------------------------------------------------------------------------
void
osxAppBridge::onFrame() {
    o_assert_dbg(this->app);
    this->app->onFrame();
}

//------------------------------------------------------------------------------
void
osxAppBridge::createWindow() {
    o_assert_dbg(nil == this->appWindow);

    // window delegate
    this->appWindowDelegate = [[oryolWindowDelegate alloc] init];
    o_assert(nil != this->appWindowDelegate);

    // window
    const NSUInteger style =
        NSTitledWindowMask |
        NSClosableWindowMask |
        NSMiniaturizableWindowMask |
        NSResizableWindowMask;
    this->appWindow = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 64, 32)
        styleMask:style
        backing:NSBackingStoreBuffered
        defer:NO];
    o_assert(nil != this->appWindow);
    [this->appWindow setAcceptsMouseMovedEvents:YES];
    [this->appWindow setRestorable:YES];
    [this->appWindow setDelegate:this->appWindowDelegate];

    // view delegate, MTKView and metal device
    this->mtkViewDelegate = [[oryolViewDelegate alloc] init];
    this->mtlDevice = MTLCreateSystemDefaultDevice();
    this->mtkView = [[MTKView alloc] init];
    [this->mtkView setPreferredFramesPerSecond:60];
    [this->mtkView setDelegate:this->mtkViewDelegate];
    [this->mtkView setDevice:this->mtlDevice];
    [this->mtkView setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [this->mtkView setDepthStencilPixelFormat:MTLPixelFormatDepth24Unorm_Stencil8];
    [this->appWindow setContentView:this->mtkView];

    // bring to front and done
    [this->appWindow makeKeyAndOrderFront:nil];
}

} // namespace _priv
} // namespace Oryol

