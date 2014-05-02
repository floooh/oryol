//------------------------------------------------------------------------------
//  iosBridge.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "iosBridge.h"
#include "Core/Macros.h"
#include "Core/Log.h"
#include "Core/App.h"
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "iosAppDelegate.h"

namespace Oryol {
namespace Core {

OryolGlobalSingletonImpl(iosBridge);

//------------------------------------------------------------------------------
iosBridge::iosBridge(App* app_) :
app(app_),
appDelegate(nil),
appWindow(nil),
eaglContext(nil),
glkView(nil),
glkViewController(nil) {
    o_assert(nullptr != this->app);
    this->SingletonEnsureUnique();
    Log::Info("iosBridge::iosBridge() called.\n");
}

//------------------------------------------------------------------------------
iosBridge::~iosBridge() {
    Log::Info("iosBridge::~iosBridge() called.\n");

    [this->eaglContext invalidate]; this->eaglContext = nil;
    [this->appWindow invalidate]; this->appWindow = nil;
    [this->appDelegate invalidate]; this->appDelegate = nil;
    [this->glkView invalidate]; this->glkView = nil;
    [this->glkViewController invalidate]; this->glkViewController = nil;
    this->app = nullptr;
}

//------------------------------------------------------------------------------
void
iosBridge::startMainLoop() {
    Log::Info("iosBridge::startMainLoop() called.\n");
    @autoreleasepool {
        int argc = 0;
        char* argv[] = {};
        UIApplicationMain(argc, argv, nil, NSStringFromClass([iosAppDelegate class]));
    }
}

//------------------------------------------------------------------------------
void
iosBridge::onDidFinishLaunching() {
    Log::Info("iosBridge::onDidFinishLaunching() called.\n");

    // get pointer to our app delegate
    this->appDelegate = [UIApplication sharedApplication].delegate;
    
    // create the app's main window
    CGRect mainScreenBounds = [[UIScreen mainScreen] bounds];
    UIWindow* win = [[UIWindow alloc] initWithFrame:mainScreenBounds];
    
    // create GL context and GLKView
    // NOTE: the drawable properties will be overridden later in iosDisplayMgr!
    this->eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    GLKView* _glkView = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _glkView.drawableColorFormat   = GLKViewDrawableColorFormatRGBA8888;
    _glkView.drawableDepthFormat   = GLKViewDrawableDepthFormat24;
    _glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
    _glkView.drawableMultisample   = GLKViewDrawableMultisampleNone;
    _glkView.context = this->eaglContext;
    _glkView.delegate = this->appDelegate;
    _glkView.enableSetNeedsDisplay = NO;
    _glkView.userInteractionEnabled = YES;
    _glkView.multipleTouchEnabled = YES;
    this->glkView = _glkView;
    [win addSubview:_glkView];
    
    // create a GLKViewController
    GLKViewController* _glkViewController = [[GLKViewController alloc] init];
    _glkViewController.view = _glkView;
    _glkViewController.preferredFramesPerSecond = 60;
    this->glkViewController = _glkViewController;
    win.rootViewController = this->glkViewController;
    
    // attach view controller and make window visible
    win.backgroundColor = [UIColor blackColor];
    this->appWindow = win;
    [this->appWindow makeKeyAndVisible];
}

//------------------------------------------------------------------------------
void
iosBridge::onWillResignActive() {
    Log::Info("iosBridge::onWillResignActive() called.\n");
}

//------------------------------------------------------------------------------
void
iosBridge::onDidEnterBackground() {
    Log::Info("iosBridge::onDidEnterBackGround() called.\n");
}

//------------------------------------------------------------------------------
void
iosBridge::onWillEnterForeGround() {
    Log::Info("iosBridge::onWillEnterForeGround() called.\n");
}

//------------------------------------------------------------------------------
void
iosBridge::onDidBecomeActive() {
    Log::Info("iosBridge::onWillBecomeActive() called.\n");
}

//------------------------------------------------------------------------------
void
iosBridge::onWillTerminate() {
    Log::Info("iosBridge::onWillTerminate() called.\n");
}

//------------------------------------------------------------------------------
void
iosBridge::onDrawRequested() {
    // this will in turn call onFrame to do the entire frame look
    o_assert(nil != this->glkView);
    [this->glkView display];
}

//------------------------------------------------------------------------------
void
iosBridge::onFrame() {
    this->app->onFrame();
}

//------------------------------------------------------------------------------
id
iosBridge::iosGetAppDelegate() const {
    o_assert(nil != this->appDelegate);
    return this->appDelegate;
}

//------------------------------------------------------------------------------
id
iosBridge::iosGetAppWindow() const {
    o_assert(nil != this->appWindow);
    return this->appWindow;
}

//------------------------------------------------------------------------------
id
iosBridge::iosGetEAGLContext() const {
    o_assert(nil != this->eaglContext);
    return this->eaglContext;
}

//------------------------------------------------------------------------------
id
iosBridge::iosGetGLKView() const {
    o_assert(nil != this->glkView);
    return this->glkView;
}

//------------------------------------------------------------------------------
id
iosBridge::iosGetGLKViewController() const {
    o_assert(nil != this->glkViewController);
    return this->glkViewController;
}

} // namespace Core
} // namespace Oryol
