//------------------------------------------------------------------------------
//  osxBridge.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "osxBridge.h"
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
    osxBridge::ptr()->onDidFinishLaunching();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return NO;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender {
    osxBridge::ptr()->onShouldTerminate();
    return NSTerminateNow;
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
    osxBridge::ptr()->onWillTerminate();
}

- (void)applicationWillBecomeActive:(NSNotification*)aNotification {
    osxBridge::ptr()->onWillBecomeActive();
}

- (void)applicationDidBecomeActive:(NSNotification*)aNotification {
    osxBridge::ptr()->onDidBecomeActive();
}

- (void)applicationWillResignActive:(NSNotification*)aNotification {
    osxBridge::ptr()->onWillResignActive();
}

- (void)applicationDidResignActive:(NSNotification*)aNotification {
    osxBridge::ptr()->onDidResignActive();
}

- (void)applicationWillHide:(NSNotification*)aNotification {
    osxBridge::ptr()->onWillHide();
}

- (void)applicationDidHide:(NSNotification*)aNotification {
    osxBridge::ptr()->onDidHide();
}

- (void)applicationWillUnhide:(NSNotification*)aNotification {
    osxBridge::ptr()->onWillUnhide();
}

- (void)applicationDidUnhide:(NSNotification*)aNotification {
    osxBridge::ptr()->onDidUnhide();
}

- (void)applicationDidChangeScreenParameters:(NSNotification*)aNotifaction {
    osxBridge::ptr()->onDidChangeScreenParameters();
}

- (void)applicationDidChangeOcclusionState:(NSNotification*)aNotification {
    osxBridge::ptr()->onDidChangeOcclusionState();
}
@end

//------------------------------------------------------------------------------
@interface oryolWindowDelegate<MTKViewDelegate> : NSObject
@end

@implementation oryolWindowDelegate
- (BOOL)windowShouldClose:(id)sender {
    osxBridge::ptr()->onWindowShouldClose();
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification {
    osxBridge::ptr()->onWindowDidResize();
}

- (void)windowDidMove:(NSNotification *)notification {
    osxBridge::ptr()->onWindowDidMove();
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    osxBridge::ptr()->onWindowDidMiniaturize();
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    osxBridge::ptr()->onWindowDidDeminiaturize();
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    osxBridge::ptr()->onWindowDidBecomeKey();
}

- (void)windowDidResignKey:(NSNotification *)notification {
    osxBridge::ptr()->onWindowDidResignKey();
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
        osxBridge::ptr()->onFrame();
    }
}
@end

namespace Oryol {
namespace _priv {

osxBridge* osxBridge::self = nullptr;

//------------------------------------------------------------------------------
osxBridge::osxBridge() :
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
osxBridge::~osxBridge() {
    o_assert(nil == this->app);
    o_assert(self);
    self = nullptr;
}

//------------------------------------------------------------------------------
osxBridge*
osxBridge::ptr() {
    o_assert_dbg(self);
    return self;
}

//------------------------------------------------------------------------------
void
osxBridge::setup(App* app_) {
    o_assert_dbg(nullptr == this->app);
    o_assert_dbg(nullptr != app_);
    Log::Info("osxBridge::setup() called\n");

    this->app = app_;
}

//------------------------------------------------------------------------------
void
osxBridge::discard() {
    o_assert_dbg(nullptr != this->app);
    Log::Info("osxBridge::discard() called\n");

    this->appWindow = nil;
    this->app = nullptr;
}

//------------------------------------------------------------------------------
bool
osxBridge::shouldClose() {
    return this->shouldCloseFlag;
}

//------------------------------------------------------------------------------
void
osxBridge::startMainLoop() {
    Log::Info("osxBridge::startMainLoop() called\n");
    [oryolApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    this->appDelegate = [[oryolAppDelegate alloc] init];
    [NSApp setDelegate:this->appDelegate];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
}

//------------------------------------------------------------------------------
void
osxBridge::onDestroy() {
    [this->mtkView setDelegate:nil];
    [this->appWindow setDelegate:nil];
    [this->appWindow orderOut:nil];
    [NSApp stop:nil];
}

//------------------------------------------------------------------------------
void
osxBridge::onDidFinishLaunching() {
    Log::Info("osxBridge::onDidFinishLaunched() called\n");
    this->createWindow();
}

//------------------------------------------------------------------------------
void
osxBridge::onShouldTerminate() {
    Log::Info("osxBridge::onShouldTerminate() called\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWillTerminate() {
    Log::Info("osxBridge::onWillTerminate() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWillBecomeActive() {
    Log::Info("osxBridge::onWillBecomeActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onDidBecomeActive() {
    Log::Info("osxBridge::onDidBecomeActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWillResignActive() {
    Log::Info("osxBridge::onWillResignActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onDidResignActive() {
    Log::Info("osxBridge::onDidResignActive() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWillHide() {
    Log::Info("osxBridge::onWillHide() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onDidHide() {
    Log::Info("osxBridge::onDidHide() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWillUnhide() {
    Log::Info("osxBridge::onWillUnhide() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onDidUnhide() {
    Log::Info("osxBridge::onDidUnhide() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onDidChangeScreenParameters() {
    Log::Info("osxBridge::onDidChangeScreenParameters() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onDidChangeOcclusionState() {
    Log::Info("osxBridge::onDidChangeOcclusionState() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowShouldClose() {
    this->shouldCloseFlag = true;
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidResize() {
    Log::Info("osxBridge::onWindowDidResize() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidMove() {
    Log::Info("osxBridge::onWindowDidMove() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidMiniaturize() {
    Log::Info("osxBridge::onWindowDidMiniaturize() called\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidDeminiaturize() {
    Log::Info("osxBridge::osxWindowDidDeminiaturize() called\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidBecomeKey() {
    Log::Info("osxBridge::osxWindowDidBecomeKey() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidResignKey() {
    Log::Info("osxBridge::osxWindowDidResignKey() called!\n");
}

//------------------------------------------------------------------------------
void
osxBridge::onFrame() {
    o_assert_dbg(this->app);
    this->app->onFrame();
}

//------------------------------------------------------------------------------
void
osxBridge::createWindow() {
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

    // the window is initially hidden until osxBridge::showWindow
    // is called from within the display manager
}

//------------------------------------------------------------------------------
void
osxBridge::showWindow() {
    o_assert_dbg(nil != this->appWindow);
    [this->appWindow makeKeyAndOrderFront:nil];
}

} // namespace _priv
} // namespace Oryol

