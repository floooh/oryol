//------------------------------------------------------------------------------
//  cocoa.mm
//  NOTE: this is largely ripped from GLFW3 (https://github.com/glfw/glfw)
//------------------------------------------------------------------------------
#include "Pre.h"
#include "cocoa.h"
#include "cocoaInputDefs.h"
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"

#import <Metal/Metal.h>
#import <MetalKit/MTKView.h>

//------------------------------------------------------------------------------
//  our NSApplication subclass
//
@interface oryolCocoaApplication : NSApplication
@end

@implementation oryolCocoaApplication
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
@interface oryolCocoaApplicationDelegate : NSObject
@end

@implementation oryolCocoaApplicationDelegate
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


//------------------------------------------------------------------------------
//  our window delegate class
//
@interface oryolCocoaWindowDelegate : NSObject
{
    Oryol::_priv::cocoa::cocoaWindowNS* window;
}

- (id)initWithContext:(Oryol::_priv::cocoa::cocoaWindowNS *)initWindow;

@end

@implementation oryolCocoaWindowDelegate

- (id)initWithContext:(Oryol::_priv::cocoa::cocoaWindowNS *)initWindow {
    self = [super init];
    if (self != nil) {
        window = initWindow;
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender {
    // FIXME
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification {
    /*
    FIXME
    if (_glfw.focusedWindow == window &&
        window->cursorMode == GLFW_CURSOR_DISABLED)
    {
        centerCursor(window);
    }

    const NSRect contentRect = [window->ns.view frame];
    const NSRect fbRect = convertRectToBacking(window, contentRect);

    _glfwInputFramebufferSize(window, fbRect.size.width, fbRect.size.height);
    _glfwInputWindowSize(window, contentRect.size.width, contentRect.size.height);
    _glfwInputWindowDamage(window);
    */
}

- (void)windowDidMove:(NSNotification *)notification {
    /*
    FIXME
    if (_glfw.focusedWindow == window &&
        window->cursorMode == GLFW_CURSOR_DISABLED)
    {
        centerCursor(window);
    }

    int x, y;
    _glfwPlatformGetWindowPos(window, &x, &y);
    _glfwInputWindowPos(window, x, y);
    */
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    // FIXME
    // _glfwInputWindowIconify(window, GL_TRUE);
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    // FIXME
    // _glfwInputWindowIconify(window, GL_FALSE);
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    // FIXME
    /*
    if (window->monitor)
        enterFullscreenMode(window);

    if (_glfw.focusedWindow == window &&
        window->cursorMode == GLFW_CURSOR_DISABLED)
    {
        centerCursor(window);
    }

    _glfwInputWindowFocus(window, GL_TRUE);
    _glfwPlatformApplyCursorMode(window);
    */
}

- (void)windowDidResignKey:(NSNotification *)notification {
    // FIXME
    /*
    if (window->monitor)
        leaveFullscreenMode(window);

    _glfwInputWindowFocus(window, GL_FALSE);
    */
}
@end

//------------------------------------------------------------------------------
//  our window class
//
@interface oryolCocoaWindow : NSWindow {}
@end

@implementation oryolCocoaWindow
- (BOOL)canBecomeKeyWindow {
    // Required for NSBorderlessWindowMask windows
    return YES;
}
@end

//------------------------------------------------------------------------------
//  our content-view class
//
@interface oryolCocoaView : MTKView {}
@end

@implementation oryolCocoaView
{
@private
    Oryol::_priv::cocoa::cocoaWindowNS* _window;
    id <MTLDevice> _device;
}

- (id)initWithContext:(Oryol::_priv::cocoa::cocoaWindowNS*)initWindow {
    self = [super initWithFrame:NSMakeRect(0, 0, 1, 1)];
    if (self != nil) {
        _window = initWindow;
        self.device = MTLCreateSystemDefaultDevice();
    }
    return self;
}

- (BOOL)isOpaque {
    return YES;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder{
    return YES;
}
@end


namespace Oryol {
namespace _priv {

static void createKeyTable(cocoa::cocoaGlobalNS& global);
static void initializeAppKit(cocoa::cocoaGlobalNS& global);

//------------------------------------------------------------------------------
void
cocoa::init() {
    this->global.autoreleasePool = [[NSAutoreleasePool alloc] init];
    this->global.delegate = nil;
    createKeyTable(this->global);

    this->window.object = nil;
    this->window.delegate = nil;
    this->window.view = nil;
    this->window.modifierFlags = 0;
    this->window.warpDeltaX = 0.0;
    this->window.warpDeltaY = 0.0;
}

//------------------------------------------------------------------------------
void
cocoa::terminate() {
    if (this->global.delegate) {
        [NSApp setDelegate:nil];
        [this->global.delegate release];
        this->global.delegate = nil;
    }
    [this->global.autoreleasePool release];
    this->global.autoreleasePool = nil;
}

//------------------------------------------------------------------------------
void
cocoa::createWindow(int width, int height, const char* title) {
    if (!NSApp) {
        initializeAppKit(this->global);
    }

    this->window.delegate = [[oryolCocoaWindowDelegate alloc] initWithContext:&this->window];
    o_assert(nil != this->window.delegate);

    // FIXME: fullscreen support
    unsigned int styleMask =
        NSTitledWindowMask |
        NSClosableWindowMask |
        NSMiniaturizableWindowMask |
        NSResizableWindowMask;

    NSRect contentRect = NSMakeRect(0, 0, width, height);
    this->window.object = [[oryolCocoaWindow alloc]
        initWithContentRect:contentRect
                  styleMask:styleMask
                    backing:NSBackingStoreBuffered
                      defer:NO];
    o_assert(nil != this->window.object);

    [this->window.object setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    [this->window.object center];
    [this->window.object setTitle:[NSString stringWithUTF8String:title]];
    [this->window.object setDelegate:this->window.delegate];
    [this->window.object setAcceptsMouseMovedEvents:YES];
    [this->window.object setRestorable:NO];

    this->window.view = [[oryolCocoaView alloc] initWithContext:&this->window];

    // FIXME FULL RETINA RESOLUTION?
    /*
    #if defined(_GLFW_USE_RETINA)
        if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
            [window->ns.view setWantsBestResolutionOpenGLSurface:YES];
    #endif
    */

    [this->window.object setContentView:this->window.view];
}

//------------------------------------------------------------------------------
void
cocoa::destroyWindow() {
    if (nil != this->window.object) {
        [this->window.object orderOut:nil];

        // FIXME: destroy metal context

        [this->window.object setDelegate:nil];

        if (nil != this->window.delegate) {
            [this->window.delegate release];
            this->window.delegate = nil;
        }

        if (nil != this->window.view) {
            [this->window.view release];
            this->window.view = nil;
        }

        [this->window.object close];
        this->window.object = nil;
    }
}

//------------------------------------------------------------------------------
void
cocoa::pollEvents() {
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
    [this->global.autoreleasePool drain];
    this->global.autoreleasePool = [[NSAutoreleasePool alloc] init];
}

//------------------------------------------------------------------------------
void
initializeAppKit(cocoa::cocoaGlobalNS& global) {
    o_assert(!NSApp);

    // Implicitly create shared NSApplication instance
    [oryolCocoaApplication sharedApplication];

    // In case we are unbundled, make us a proper UI application
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // There can only be one application delegate, but we allocate it the
    // first time a window is created to keep all window code in this file
    global.delegate = [[oryolCocoaApplicationDelegate alloc] init];
    o_assert(nil != global.delegate);

    [NSApp setDelegate:global.delegate];
    [NSApp run];
}

//------------------------------------------------------------------------------
void
createKeyTable(cocoa::cocoaGlobalNS& global) {
    Memory::Clear(global.publicKeys, sizeof(global.publicKeys));
    global.publicKeys[0x1D] = ORYOL_COCOA_KEY_0;
    global.publicKeys[0x12] = ORYOL_COCOA_KEY_1;
    global.publicKeys[0x13] = ORYOL_COCOA_KEY_2;
    global.publicKeys[0x14] = ORYOL_COCOA_KEY_3;
    global.publicKeys[0x15] = ORYOL_COCOA_KEY_4;
    global.publicKeys[0x17] = ORYOL_COCOA_KEY_5;
    global.publicKeys[0x16] = ORYOL_COCOA_KEY_6;
    global.publicKeys[0x1A] = ORYOL_COCOA_KEY_7;
    global.publicKeys[0x1C] = ORYOL_COCOA_KEY_8;
    global.publicKeys[0x19] = ORYOL_COCOA_KEY_9;
    global.publicKeys[0x00] = ORYOL_COCOA_KEY_A;
    global.publicKeys[0x0B] = ORYOL_COCOA_KEY_B;
    global.publicKeys[0x08] = ORYOL_COCOA_KEY_C;
    global.publicKeys[0x02] = ORYOL_COCOA_KEY_D;
    global.publicKeys[0x0E] = ORYOL_COCOA_KEY_E;
    global.publicKeys[0x03] = ORYOL_COCOA_KEY_F;
    global.publicKeys[0x05] = ORYOL_COCOA_KEY_G;
    global.publicKeys[0x04] = ORYOL_COCOA_KEY_H;
    global.publicKeys[0x22] = ORYOL_COCOA_KEY_I;
    global.publicKeys[0x26] = ORYOL_COCOA_KEY_J;
    global.publicKeys[0x28] = ORYOL_COCOA_KEY_K;
    global.publicKeys[0x25] = ORYOL_COCOA_KEY_L;
    global.publicKeys[0x2E] = ORYOL_COCOA_KEY_M;
    global.publicKeys[0x2D] = ORYOL_COCOA_KEY_N;
    global.publicKeys[0x1F] = ORYOL_COCOA_KEY_O;
    global.publicKeys[0x23] = ORYOL_COCOA_KEY_P;
    global.publicKeys[0x0C] = ORYOL_COCOA_KEY_Q;
    global.publicKeys[0x0F] = ORYOL_COCOA_KEY_R;
    global.publicKeys[0x01] = ORYOL_COCOA_KEY_S;
    global.publicKeys[0x11] = ORYOL_COCOA_KEY_T;
    global.publicKeys[0x20] = ORYOL_COCOA_KEY_U;
    global.publicKeys[0x09] = ORYOL_COCOA_KEY_V;
    global.publicKeys[0x0D] = ORYOL_COCOA_KEY_W;
    global.publicKeys[0x07] = ORYOL_COCOA_KEY_X;
    global.publicKeys[0x10] = ORYOL_COCOA_KEY_Y;
    global.publicKeys[0x06] = ORYOL_COCOA_KEY_Z;

    global.publicKeys[0x27] = ORYOL_COCOA_KEY_APOSTROPHE;
    global.publicKeys[0x2A] = ORYOL_COCOA_KEY_BACKSLASH;
    global.publicKeys[0x2B] = ORYOL_COCOA_KEY_COMMA;
    global.publicKeys[0x18] = ORYOL_COCOA_KEY_EQUAL;
    global.publicKeys[0x32] = ORYOL_COCOA_KEY_GRAVE_ACCENT;
    global.publicKeys[0x21] = ORYOL_COCOA_KEY_LEFT_BRACKET;
    global.publicKeys[0x1B] = ORYOL_COCOA_KEY_MINUS;
    global.publicKeys[0x2F] = ORYOL_COCOA_KEY_PERIOD;
    global.publicKeys[0x1E] = ORYOL_COCOA_KEY_RIGHT_BRACKET;
    global.publicKeys[0x29] = ORYOL_COCOA_KEY_SEMICOLON;
    global.publicKeys[0x2C] = ORYOL_COCOA_KEY_SLASH;
    global.publicKeys[0x0A] = ORYOL_COCOA_KEY_WORLD_1;

    global.publicKeys[0x33] = ORYOL_COCOA_KEY_BACKSPACE;
    global.publicKeys[0x39] = ORYOL_COCOA_KEY_CAPS_LOCK;
    global.publicKeys[0x75] = ORYOL_COCOA_KEY_DELETE;
    global.publicKeys[0x7D] = ORYOL_COCOA_KEY_DOWN;
    global.publicKeys[0x77] = ORYOL_COCOA_KEY_END;
    global.publicKeys[0x24] = ORYOL_COCOA_KEY_ENTER;
    global.publicKeys[0x35] = ORYOL_COCOA_KEY_ESCAPE;
    global.publicKeys[0x7A] = ORYOL_COCOA_KEY_F1;
    global.publicKeys[0x78] = ORYOL_COCOA_KEY_F2;
    global.publicKeys[0x63] = ORYOL_COCOA_KEY_F3;
    global.publicKeys[0x76] = ORYOL_COCOA_KEY_F4;
    global.publicKeys[0x60] = ORYOL_COCOA_KEY_F5;
    global.publicKeys[0x61] = ORYOL_COCOA_KEY_F6;
    global.publicKeys[0x62] = ORYOL_COCOA_KEY_F7;
    global.publicKeys[0x64] = ORYOL_COCOA_KEY_F8;
    global.publicKeys[0x65] = ORYOL_COCOA_KEY_F9;
    global.publicKeys[0x6D] = ORYOL_COCOA_KEY_F10;
    global.publicKeys[0x67] = ORYOL_COCOA_KEY_F11;
    global.publicKeys[0x6F] = ORYOL_COCOA_KEY_F12;
    global.publicKeys[0x69] = ORYOL_COCOA_KEY_F13;
    global.publicKeys[0x6B] = ORYOL_COCOA_KEY_F14;
    global.publicKeys[0x71] = ORYOL_COCOA_KEY_F15;
    global.publicKeys[0x6A] = ORYOL_COCOA_KEY_F16;
    global.publicKeys[0x40] = ORYOL_COCOA_KEY_F17;
    global.publicKeys[0x4F] = ORYOL_COCOA_KEY_F18;
    global.publicKeys[0x50] = ORYOL_COCOA_KEY_F19;
    global.publicKeys[0x5A] = ORYOL_COCOA_KEY_F20;
    global.publicKeys[0x73] = ORYOL_COCOA_KEY_HOME;
    global.publicKeys[0x72] = ORYOL_COCOA_KEY_INSERT;
    global.publicKeys[0x7B] = ORYOL_COCOA_KEY_LEFT;
    global.publicKeys[0x3A] = ORYOL_COCOA_KEY_LEFT_ALT;
    global.publicKeys[0x3B] = ORYOL_COCOA_KEY_LEFT_CONTROL;
    global.publicKeys[0x38] = ORYOL_COCOA_KEY_LEFT_SHIFT;
    global.publicKeys[0x37] = ORYOL_COCOA_KEY_LEFT_SUPER;
    global.publicKeys[0x6E] = ORYOL_COCOA_KEY_MENU;
    global.publicKeys[0x47] = ORYOL_COCOA_KEY_NUM_LOCK;
    global.publicKeys[0x79] = ORYOL_COCOA_KEY_PAGE_DOWN;
    global.publicKeys[0x74] = ORYOL_COCOA_KEY_PAGE_UP;
    global.publicKeys[0x7C] = ORYOL_COCOA_KEY_RIGHT;
    global.publicKeys[0x3D] = ORYOL_COCOA_KEY_RIGHT_ALT;
    global.publicKeys[0x3E] = ORYOL_COCOA_KEY_RIGHT_CONTROL;
    global.publicKeys[0x3C] = ORYOL_COCOA_KEY_RIGHT_SHIFT;
    global.publicKeys[0x36] = ORYOL_COCOA_KEY_RIGHT_SUPER;
    global.publicKeys[0x31] = ORYOL_COCOA_KEY_SPACE;
    global.publicKeys[0x30] = ORYOL_COCOA_KEY_TAB;
    global.publicKeys[0x7E] = ORYOL_COCOA_KEY_UP;

    global.publicKeys[0x52] = ORYOL_COCOA_KEY_KP_0;
    global.publicKeys[0x53] = ORYOL_COCOA_KEY_KP_1;
    global.publicKeys[0x54] = ORYOL_COCOA_KEY_KP_2;
    global.publicKeys[0x55] = ORYOL_COCOA_KEY_KP_3;
    global.publicKeys[0x56] = ORYOL_COCOA_KEY_KP_4;
    global.publicKeys[0x57] = ORYOL_COCOA_KEY_KP_5;
    global.publicKeys[0x58] = ORYOL_COCOA_KEY_KP_6;
    global.publicKeys[0x59] = ORYOL_COCOA_KEY_KP_7;
    global.publicKeys[0x5B] = ORYOL_COCOA_KEY_KP_8;
    global.publicKeys[0x5C] = ORYOL_COCOA_KEY_KP_9;
    global.publicKeys[0x45] = ORYOL_COCOA_KEY_KP_ADD;
    global.publicKeys[0x41] = ORYOL_COCOA_KEY_KP_DECIMAL;
    global.publicKeys[0x4B] = ORYOL_COCOA_KEY_KP_DIVIDE;
    global.publicKeys[0x4C] = ORYOL_COCOA_KEY_KP_ENTER;
    global.publicKeys[0x51] = ORYOL_COCOA_KEY_KP_EQUAL;
    global.publicKeys[0x43] = ORYOL_COCOA_KEY_KP_MULTIPLY;
    global.publicKeys[0x4E] = ORYOL_COCOA_KEY_KP_SUBTRACT;
}

} // namespace _priv
} // namespace Oryol
