//------------------------------------------------------------------------------
//  osxBridge.mm
//  WARNING: all of this is boring window management code, and not particularly
//  pretty, it is mostly a mish-mash of self-rolles OSX window creation plus
//  event handling ripped from GLFW.
//
//  Once GLFW supports a 'GL-context-less' mode, this code will be revisited.
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
    if ([event type] == NSEventTypeKeyUp && ([event modifierFlags] & NSEventModifierFlagCommand)) {
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
@end

//------------------------------------------------------------------------------
@interface oryolWindowDelegate<NSWindowDelegate> : NSObject
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
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    // FIXME(?)
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    @autoreleasepool {
        osxBridge::ptr()->onFrame();
    }
}
@end

//------------------------------------------------------------------------------
static int translateFlags(NSUInteger flags) {
    int mods = 0;
    if (flags & NSEventModifierFlagShift) {
        mods |= ORYOL_OSXBRIDGE_MOD_SHIFT;
    }
    if (flags & NSEventModifierFlagControl) {
        mods |= ORYOL_OSXBRIDGE_MOD_CONTROL;
    }
    if (flags & NSEventModifierFlagOption) {
        mods |= ORYOL_OSXBRIDGE_MOD_ALT;
    }
    if (flags & NSEventModifierFlagCommand) {
        mods |= ORYOL_OSXBRIDGE_MOD_SUPER;
    }
    return mods;
}

static int translateKey(unsigned int key) {
    osxBridge* self = osxBridge::ptr();
    if (key >= sizeof(self->publicKeys) / sizeof(self->publicKeys[0])) {
        return ORYOL_OSXBRIDGE_KEY_UNKNOWN;
    }
    return self->publicKeys[key];
}

//------------------------------------------------------------------------------
@interface oryolMTKView: MTKView
@end

@implementation oryolMTKView

- (BOOL)isOpaque {
    return YES;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
    osxBridge::ptr()->onMouseClick(
        ORYOL_OSXBRIDGE_MOUSE_BUTTON_LEFT,
        ORYOL_OSXBRIDGE_PRESS,
        translateFlags([event modifierFlags]));
}

- (void)mouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event {
    osxBridge::ptr()->onMouseClick(
        ORYOL_OSXBRIDGE_MOUSE_BUTTON_LEFT,
        ORYOL_OSXBRIDGE_RELEASE,
        translateFlags([event modifierFlags]));
}

- (void)mouseMoved:(NSEvent *)event {
    osxBridge* bridge = osxBridge::ptr();
    if (bridge->cursorMode == ORYOL_OSXBRIDGE_CURSOR_DISABLED) {
        bridge->onCursorMotion([event deltaX] - bridge->warpDeltaX,
                               [event deltaY] - bridge->warpDeltaY);
    }
    else {
        const NSRect contentRect = [bridge->mtkView frame];
        const NSPoint pos = [event locationInWindow];
        bridge->onCursorMotion(pos.x, contentRect.size.height - pos.y);
    }
    bridge->warpDeltaX = 0;
    bridge->warpDeltaY = 0;
}

- (void)rightMouseDown:(NSEvent *)event {
    osxBridge::ptr()->onMouseClick(
        ORYOL_OSXBRIDGE_MOUSE_BUTTON_RIGHT,
        ORYOL_OSXBRIDGE_PRESS,
        translateFlags([event modifierFlags]));
}

- (void)rightMouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event {
    osxBridge::ptr()->onMouseClick(
        ORYOL_OSXBRIDGE_MOUSE_BUTTON_RIGHT,
        ORYOL_OSXBRIDGE_RELEASE,
        translateFlags([event modifierFlags]));
}

- (void)otherMouseDown:(NSEvent *)event {
    osxBridge::ptr()->onMouseClick(
        (int) [event buttonNumber],
        ORYOL_OSXBRIDGE_PRESS,
        translateFlags([event modifierFlags]));
}

- (void)otherMouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event {
    osxBridge::ptr()->onMouseClick(
        (int) [event buttonNumber],
        ORYOL_OSXBRIDGE_RELEASE,
        translateFlags([event modifierFlags]));
}

- (void)mouseExited:(NSEvent *)event {
    osxBridge::ptr()->onCursorEnter(false);
}

- (void)mouseEntered:(NSEvent *)event {
    osxBridge::ptr()->onCursorEnter(true);
}

- (void)keyDown:(NSEvent *)event {
    osxBridge* bridge = osxBridge::ptr();
    const int key = translateKey([event keyCode]);
    const int mods = translateFlags([event modifierFlags]);

    bridge->onInputKey(key, [event keyCode], ORYOL_OSXBRIDGE_PRESS, mods);

    NSString* characters = [event characters];
    NSUInteger i, length = [characters length];
    const int plain = !(mods & ORYOL_OSXBRIDGE_MOD_SUPER);
    for (i = 0;  i < length;  i++) {
        const unichar codepoint = [characters characterAtIndex:i];
        if ((codepoint & 0xff00) == 0xf700) {
            continue;
        }
        bridge->onInputChar(codepoint, mods, plain);
    }
}

- (void)flagsChanged:(NSEvent *)event {
    osxBridge* bridge = osxBridge::ptr();
    int action;
    const unsigned int modifierFlags = [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask;
    const int key = translateKey([event keyCode]);
    const int mods = translateFlags(modifierFlags);

    if (modifierFlags == bridge->modifierFlags) {
        if (bridge->keys[key] == ORYOL_OSXBRIDGE_PRESS) {
            action = ORYOL_OSXBRIDGE_RELEASE;
        }
        else {
            action = ORYOL_OSXBRIDGE_PRESS;
        }
    }
    else if (modifierFlags > bridge->modifierFlags) {
        action = ORYOL_OSXBRIDGE_PRESS;
    }
    else {
        action = ORYOL_OSXBRIDGE_RELEASE;
    }
    bridge->modifierFlags = modifierFlags;
    bridge->onInputKey(key, [event keyCode], action, mods);
}

- (void)keyUp:(NSEvent *)event {
    const int key = translateKey([event keyCode]);
    const int mods = translateFlags([event modifierFlags]);
    osxBridge::ptr()->onInputKey(key, [event keyCode], ORYOL_OSXBRIDGE_RELEASE, mods);
}

- (void)scrollWheel:(NSEvent *)event {
    double deltaX = [event scrollingDeltaX];
    double deltaY = [event scrollingDeltaY];
    if ([event hasPreciseScrollingDeltas]) {
        deltaX *= 0.1;
        deltaY *= 0.1;
    }
    else {
        deltaX = [event deltaX];
        deltaY = [event deltaY];
    }
    if (std::fabs(deltaX) > 0.0 || std::fabs(deltaY) > 0.0) {
        osxBridge::ptr()->onInputScroll(deltaX, deltaY);
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
mtkView(nil),
cursorMode(ORYOL_OSXBRIDGE_CURSOR_NORMAL),
modifierFlags(0),
warpDeltaX(0.0),
warpDeltaY(0.0),
cursorPosX(0.0),
cursorPosY(0.0),
winCursorPosX(0.0),
winCursorPosY(0.0),
mouseScale(1.0) {
    o_assert(nullptr == self);
    self = this;
    Memory::Clear(this->keys, sizeof(this->keys));
    Memory::Clear(this->mouseButtons, sizeof(this->mouseButtons));
    this->setupKeyTable();
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
osxBridge::centerCursor() {
    int width, height;
    this->cocoaGetWindowSize(&width, &height);
    this->cocoaSetCursorPos(width/2, height/2);
}

//------------------------------------------------------------------------------
void
osxBridge::setCursorMode(int newMode) {
    // from GLFW cursor mode
    o_assert((ORYOL_OSXBRIDGE_CURSOR_NORMAL == newMode) ||
             (ORYOL_OSXBRIDGE_CURSOR_HIDDEN == newMode) ||
             (ORYOL_OSXBRIDGE_CURSOR_DISABLED == newMode));
    const int oldMode = this->cursorMode;
    if (newMode == oldMode) {
        return;
    }
    this->cursorMode = newMode;
    if (oldMode == ORYOL_OSXBRIDGE_CURSOR_DISABLED) {
        this->cocoaSetCursorPos(this->cursorPosX, this->cursorPosY);
    }
    else if (newMode == ORYOL_OSXBRIDGE_CURSOR_DISABLED) {
        this->cocoaGetCursorPos(&this->cursorPosX, &this->cursorPosY);
        this->winCursorPosX = this->cursorPosX;
        this->winCursorPosY = this->cursorPosY;
        this->centerCursor();
    }
    this->cocoaSetCursorMode(this->cursorMode);
}

//------------------------------------------------------------------------------
int
osxBridge::getCursorMode() const {
    return this->cursorMode;
}

//------------------------------------------------------------------------------
void
osxBridge::getCursorPos(double* xpos, double* ypos) {
    if (xpos) {
        *xpos = 0;
    }
    if (ypos) {
        *ypos = 0;
    }
    if (this->cursorMode == ORYOL_OSXBRIDGE_CURSOR_DISABLED) {
        if (xpos) {
            *xpos = this->winCursorPosX;
        }
        if (ypos) {
            *ypos = this->winCursorPosY;
        }
    }
    else {
        this->cocoaGetCursorPos(xpos, ypos);
    }
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
    this->createWindow();
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowShouldClose() {
    this->shouldCloseFlag = true;
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidResize() {
    if (ORYOL_OSXBRIDGE_CURSOR_DISABLED == this->cursorMode) {
        this->centerCursor();
    }
    const NSRect contentRect = [this->mtkView frame];    
    if (this->callbacks.fbsize) {
        this->callbacks.fbsize(contentRect.size.width, contentRect.size.height);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidMove() {
    if (ORYOL_OSXBRIDGE_CURSOR_DISABLED == this->cursorMode) {
        this->centerCursor();
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidMiniaturize() {
    if (this->callbacks.iconify) {
        this->callbacks.iconify(true);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidDeminiaturize() {
    if (this->callbacks.iconify) {
        this->callbacks.iconify(false);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidBecomeKey() {
    if (ORYOL_OSXBRIDGE_CURSOR_DISABLED == this->cursorMode) {
        this->centerCursor();
    }
    if (this->callbacks.focus) {
        this->callbacks.focus(true);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onWindowDidResignKey() {
    if (this->callbacks.focus) {
        this->callbacks.focus(false);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onMouseClick(int button, int action, int mods) {
    if (button < 0 || button > ORYOL_OSXBRIDGE_MOUSE_BUTTON_LAST) {
        return;
    }
    this->mouseButtons[button] = (char) action;
    if (this->callbacks.mouseButton) {
        this->callbacks.mouseButton(button, action, mods);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onCursorMotion(double x, double y) {
    if (this->cursorMode == ORYOL_OSXBRIDGE_CURSOR_DISABLED) {
        if ((x == 0.0) && (y == 0.0)) {
            return;
        }
        this->winCursorPosX += x;
        this->winCursorPosY += y;
        x = this->winCursorPosX;
        y = this->winCursorPosY;
    }
    if (this->callbacks.cursorPos) {
        this->callbacks.cursorPos(x * this->mouseScale, y * this->mouseScale);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onCursorEnter(bool entered) {
    if (this->callbacks.cursorEnter) {
        this->callbacks.cursorEnter(entered);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onInputKey(int key, int scancode, int action, int mods) {
    if ((key >= 0) && (key <= ORYOL_OSXBRIDGE_KEY_LAST)) {
        bool repeated = false;
        if ((action == ORYOL_OSXBRIDGE_RELEASE) && (this->keys[key] == ORYOL_OSXBRIDGE_RELEASE)) {
            return;
        }

        if ((action == ORYOL_OSXBRIDGE_PRESS) && (this->keys[key] == ORYOL_OSXBRIDGE_PRESS)) {
            repeated = true;
        }
        this->keys[key] = (char) action;
        if (repeated) {
            action = ORYOL_OSXBRIDGE_REPEAT;
        }
    }
    if (this->callbacks.key) {
        this->callbacks.key(key, scancode, action, mods);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onInputChar(unsigned int codepoint, int mods, int plain) {
    if ((codepoint < 32) || ((codepoint > 126) && (codepoint < 160))) {
        return;
    }
    if (this->callbacks.charmods) {
        this->callbacks.charmods(codepoint, mods);
    }
    if (plain) {
        if (this->callbacks.character) {
            this->callbacks.character(codepoint);
        }
    }
}

//------------------------------------------------------------------------------
void
osxBridge::onInputScroll(double xoffset, double yoffset) {
    if (this->callbacks.scroll) {
        this->callbacks.scroll(xoffset, yoffset);
    }
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
        NSWindowStyleMaskTitled |
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable;
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
    this->mtkView = [[oryolMTKView alloc] init];
    [this->mtkView setPreferredFramesPerSecond:60];
    [this->mtkView setDelegate:this->mtkViewDelegate];
    [this->mtkView setDevice:this->mtlDevice];
    [this->mtkView setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [this->mtkView setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
    [this->appWindow setContentView:this->mtkView];

    // setup a blank disabled cursor
    NSImage* data = [[NSImage alloc] initWithSize:NSMakeSize(16, 16)];
    this->disabledCursor = [[NSCursor alloc] initWithImage:data hotSpot:NSZeroPoint];

    // the window is initially hidden until osxBridge::showWindow
    // is called from within the display manager
}

//------------------------------------------------------------------------------
void
osxBridge::showWindow() {
    o_assert_dbg(nil != this->appWindow);
    [this->appWindow makeKeyAndOrderFront:nil];
}

// Transforms the specified y-coordinate between the CG display and NS screen
// coordinate systems
//
static float transformY(float y)
{
    return CGDisplayBounds(CGMainDisplayID()).size.height - y;
}

//------------------------------------------------------------------------------
void
osxBridge::cocoaSetCursorPos(double x, double y) {
    this->cocoaSetCursorMode(this->cursorMode);
    const NSRect contentRect = [this->mtkView frame];
    const NSPoint pos = [this->appWindow mouseLocationOutsideOfEventStream];
    this->warpDeltaX += x - pos.x;
    this->warpDeltaY += y - contentRect.size.height + pos.y;
    const NSRect localRect = NSMakeRect(x, contentRect.size.height - y - 1, 0, 0);
    const NSRect globalRect = [this->appWindow convertRectToScreen:localRect];
    const NSPoint globalPoint = globalRect.origin;
    CGWarpMouseCursorPosition(CGPointMake(globalPoint.x, transformY(globalPoint.y)));
}

//------------------------------------------------------------------------------
void
osxBridge::cocoaGetCursorPos(double* xpos, double* ypos) {
    const NSRect contentRect = [this->mtkView frame];
    const NSPoint pos = [this->appWindow mouseLocationOutsideOfEventStream];
    if (xpos) {
        *xpos = pos.x;
    }
    if (ypos) {
        *ypos = contentRect.size.height - pos.y - 1;
    }
}

//------------------------------------------------------------------------------
void
osxBridge::cocoaSetCursorMode(int mode) {
    if (mode == ORYOL_OSXBRIDGE_CURSOR_NORMAL) {
        [[NSCursor arrowCursor] set];
    }
    else {
        [(NSCursor*)this->disabledCursor set];
    }
    if (mode == ORYOL_OSXBRIDGE_CURSOR_DISABLED) {
        CGAssociateMouseAndMouseCursorPosition(false);
    }
    else {
        CGAssociateMouseAndMouseCursorPosition(true);
    }
}

//------------------------------------------------------------------------------
void
osxBridge::cocoaGetWindowSize(int* width, int* height) {
    const NSRect contentRect = [this->mtkView frame];
    if (width) {
        *width = contentRect.size.width;
    }
    if (height) {
        *height = contentRect.size.height;
    }
}

//------------------------------------------------------------------------------
void
osxBridge::setupKeyTable() {
    std::memset(this->publicKeys, -1, sizeof(this->publicKeys));

    this->publicKeys[0x1D] = ORYOL_OSXBRIDGE_KEY_0;
    this->publicKeys[0x12] = ORYOL_OSXBRIDGE_KEY_1;
    this->publicKeys[0x13] = ORYOL_OSXBRIDGE_KEY_2;
    this->publicKeys[0x14] = ORYOL_OSXBRIDGE_KEY_3;
    this->publicKeys[0x15] = ORYOL_OSXBRIDGE_KEY_4;
    this->publicKeys[0x17] = ORYOL_OSXBRIDGE_KEY_5;
    this->publicKeys[0x16] = ORYOL_OSXBRIDGE_KEY_6;
    this->publicKeys[0x1A] = ORYOL_OSXBRIDGE_KEY_7;
    this->publicKeys[0x1C] = ORYOL_OSXBRIDGE_KEY_8;
    this->publicKeys[0x19] = ORYOL_OSXBRIDGE_KEY_9;
    this->publicKeys[0x00] = ORYOL_OSXBRIDGE_KEY_A;
    this->publicKeys[0x0B] = ORYOL_OSXBRIDGE_KEY_B;
    this->publicKeys[0x08] = ORYOL_OSXBRIDGE_KEY_C;
    this->publicKeys[0x02] = ORYOL_OSXBRIDGE_KEY_D;
    this->publicKeys[0x0E] = ORYOL_OSXBRIDGE_KEY_E;
    this->publicKeys[0x03] = ORYOL_OSXBRIDGE_KEY_F;
    this->publicKeys[0x05] = ORYOL_OSXBRIDGE_KEY_G;
    this->publicKeys[0x04] = ORYOL_OSXBRIDGE_KEY_H;
    this->publicKeys[0x22] = ORYOL_OSXBRIDGE_KEY_I;
    this->publicKeys[0x26] = ORYOL_OSXBRIDGE_KEY_J;
    this->publicKeys[0x28] = ORYOL_OSXBRIDGE_KEY_K;
    this->publicKeys[0x25] = ORYOL_OSXBRIDGE_KEY_L;
    this->publicKeys[0x2E] = ORYOL_OSXBRIDGE_KEY_M;
    this->publicKeys[0x2D] = ORYOL_OSXBRIDGE_KEY_N;
    this->publicKeys[0x1F] = ORYOL_OSXBRIDGE_KEY_O;
    this->publicKeys[0x23] = ORYOL_OSXBRIDGE_KEY_P;
    this->publicKeys[0x0C] = ORYOL_OSXBRIDGE_KEY_Q;
    this->publicKeys[0x0F] = ORYOL_OSXBRIDGE_KEY_R;
    this->publicKeys[0x01] = ORYOL_OSXBRIDGE_KEY_S;
    this->publicKeys[0x11] = ORYOL_OSXBRIDGE_KEY_T;
    this->publicKeys[0x20] = ORYOL_OSXBRIDGE_KEY_U;
    this->publicKeys[0x09] = ORYOL_OSXBRIDGE_KEY_V;
    this->publicKeys[0x0D] = ORYOL_OSXBRIDGE_KEY_W;
    this->publicKeys[0x07] = ORYOL_OSXBRIDGE_KEY_X;
    this->publicKeys[0x10] = ORYOL_OSXBRIDGE_KEY_Y;
    this->publicKeys[0x06] = ORYOL_OSXBRIDGE_KEY_Z;
    this->publicKeys[0x27] = ORYOL_OSXBRIDGE_KEY_APOSTROPHE;
    this->publicKeys[0x2A] = ORYOL_OSXBRIDGE_KEY_BACKSLASH;
    this->publicKeys[0x2B] = ORYOL_OSXBRIDGE_KEY_COMMA;
    this->publicKeys[0x18] = ORYOL_OSXBRIDGE_KEY_EQUAL;
    this->publicKeys[0x32] = ORYOL_OSXBRIDGE_KEY_GRAVE_ACCENT;
    this->publicKeys[0x21] = ORYOL_OSXBRIDGE_KEY_LEFT_BRACKET;
    this->publicKeys[0x1B] = ORYOL_OSXBRIDGE_KEY_MINUS;
    this->publicKeys[0x2F] = ORYOL_OSXBRIDGE_KEY_PERIOD;
    this->publicKeys[0x1E] = ORYOL_OSXBRIDGE_KEY_RIGHT_BRACKET;
    this->publicKeys[0x29] = ORYOL_OSXBRIDGE_KEY_SEMICOLON;
    this->publicKeys[0x2C] = ORYOL_OSXBRIDGE_KEY_SLASH;
    this->publicKeys[0x0A] = ORYOL_OSXBRIDGE_KEY_WORLD_1;
    this->publicKeys[0x33] = ORYOL_OSXBRIDGE_KEY_BACKSPACE;
    this->publicKeys[0x39] = ORYOL_OSXBRIDGE_KEY_CAPS_LOCK;
    this->publicKeys[0x75] = ORYOL_OSXBRIDGE_KEY_DELETE;
    this->publicKeys[0x7D] = ORYOL_OSXBRIDGE_KEY_DOWN;
    this->publicKeys[0x77] = ORYOL_OSXBRIDGE_KEY_END;
    this->publicKeys[0x24] = ORYOL_OSXBRIDGE_KEY_ENTER;
    this->publicKeys[0x35] = ORYOL_OSXBRIDGE_KEY_ESCAPE;
    this->publicKeys[0x7A] = ORYOL_OSXBRIDGE_KEY_F1;
    this->publicKeys[0x78] = ORYOL_OSXBRIDGE_KEY_F2;
    this->publicKeys[0x63] = ORYOL_OSXBRIDGE_KEY_F3;
    this->publicKeys[0x76] = ORYOL_OSXBRIDGE_KEY_F4;
    this->publicKeys[0x60] = ORYOL_OSXBRIDGE_KEY_F5;
    this->publicKeys[0x61] = ORYOL_OSXBRIDGE_KEY_F6;
    this->publicKeys[0x62] = ORYOL_OSXBRIDGE_KEY_F7;
    this->publicKeys[0x64] = ORYOL_OSXBRIDGE_KEY_F8;
    this->publicKeys[0x65] = ORYOL_OSXBRIDGE_KEY_F9;
    this->publicKeys[0x6D] = ORYOL_OSXBRIDGE_KEY_F10;
    this->publicKeys[0x67] = ORYOL_OSXBRIDGE_KEY_F11;
    this->publicKeys[0x6F] = ORYOL_OSXBRIDGE_KEY_F12;
    this->publicKeys[0x69] = ORYOL_OSXBRIDGE_KEY_F13;
    this->publicKeys[0x6B] = ORYOL_OSXBRIDGE_KEY_F14;
    this->publicKeys[0x71] = ORYOL_OSXBRIDGE_KEY_F15;
    this->publicKeys[0x6A] = ORYOL_OSXBRIDGE_KEY_F16;
    this->publicKeys[0x40] = ORYOL_OSXBRIDGE_KEY_F17;
    this->publicKeys[0x4F] = ORYOL_OSXBRIDGE_KEY_F18;
    this->publicKeys[0x50] = ORYOL_OSXBRIDGE_KEY_F19;
    this->publicKeys[0x5A] = ORYOL_OSXBRIDGE_KEY_F20;
    this->publicKeys[0x73] = ORYOL_OSXBRIDGE_KEY_HOME;
    this->publicKeys[0x72] = ORYOL_OSXBRIDGE_KEY_INSERT;
    this->publicKeys[0x7B] = ORYOL_OSXBRIDGE_KEY_LEFT;
    this->publicKeys[0x3A] = ORYOL_OSXBRIDGE_KEY_LEFT_ALT;
    this->publicKeys[0x3B] = ORYOL_OSXBRIDGE_KEY_LEFT_CONTROL;
    this->publicKeys[0x38] = ORYOL_OSXBRIDGE_KEY_LEFT_SHIFT;
    this->publicKeys[0x37] = ORYOL_OSXBRIDGE_KEY_LEFT_SUPER;
    this->publicKeys[0x6E] = ORYOL_OSXBRIDGE_KEY_MENU;
    this->publicKeys[0x47] = ORYOL_OSXBRIDGE_KEY_NUM_LOCK;
    this->publicKeys[0x79] = ORYOL_OSXBRIDGE_KEY_PAGE_DOWN;
    this->publicKeys[0x74] = ORYOL_OSXBRIDGE_KEY_PAGE_UP;
    this->publicKeys[0x7C] = ORYOL_OSXBRIDGE_KEY_RIGHT;
    this->publicKeys[0x3D] = ORYOL_OSXBRIDGE_KEY_RIGHT_ALT;
    this->publicKeys[0x3E] = ORYOL_OSXBRIDGE_KEY_RIGHT_CONTROL;
    this->publicKeys[0x3C] = ORYOL_OSXBRIDGE_KEY_RIGHT_SHIFT;
    this->publicKeys[0x36] = ORYOL_OSXBRIDGE_KEY_RIGHT_SUPER;
    this->publicKeys[0x31] = ORYOL_OSXBRIDGE_KEY_SPACE;
    this->publicKeys[0x30] = ORYOL_OSXBRIDGE_KEY_TAB;
    this->publicKeys[0x7E] = ORYOL_OSXBRIDGE_KEY_UP;
    this->publicKeys[0x52] = ORYOL_OSXBRIDGE_KEY_KP_0;
    this->publicKeys[0x53] = ORYOL_OSXBRIDGE_KEY_KP_1;
    this->publicKeys[0x54] = ORYOL_OSXBRIDGE_KEY_KP_2;
    this->publicKeys[0x55] = ORYOL_OSXBRIDGE_KEY_KP_3;
    this->publicKeys[0x56] = ORYOL_OSXBRIDGE_KEY_KP_4;
    this->publicKeys[0x57] = ORYOL_OSXBRIDGE_KEY_KP_5;
    this->publicKeys[0x58] = ORYOL_OSXBRIDGE_KEY_KP_6;
    this->publicKeys[0x59] = ORYOL_OSXBRIDGE_KEY_KP_7;
    this->publicKeys[0x5B] = ORYOL_OSXBRIDGE_KEY_KP_8;
    this->publicKeys[0x5C] = ORYOL_OSXBRIDGE_KEY_KP_9;
    this->publicKeys[0x45] = ORYOL_OSXBRIDGE_KEY_KP_ADD;
    this->publicKeys[0x41] = ORYOL_OSXBRIDGE_KEY_KP_DECIMAL;
    this->publicKeys[0x4B] = ORYOL_OSXBRIDGE_KEY_KP_DIVIDE;
    this->publicKeys[0x4C] = ORYOL_OSXBRIDGE_KEY_KP_ENTER;
    this->publicKeys[0x51] = ORYOL_OSXBRIDGE_KEY_KP_EQUAL;
    this->publicKeys[0x43] = ORYOL_OSXBRIDGE_KEY_KP_MULTIPLY;
    this->publicKeys[0x4E] = ORYOL_OSXBRIDGE_KEY_KP_SUBTRACT;
}

} // namespace _priv
} // namespace Oryol

