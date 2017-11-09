//------------------------------------------------------------------------------
//  iosBridge.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "iosBridge.h"
#include "Core/Log.h"
#include "Core/App.h"

//------------------------------------------------------------------------------
/**
    IOSAppDelegate wrapper
*/
using namespace Oryol;
using namespace _priv;

@interface iosAppDelegate : NSObject
@end

@implementation iosAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    iosBridge::ptr()->onDidFinishLaunching();
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    iosBridge::ptr()->onWillResignActive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    iosBridge::ptr()->onDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    iosBridge::ptr()->onWillEnterForeGround();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    iosBridge::ptr()->onDidBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    iosBridge::ptr()->onWillTerminate();
}

#if ORYOL_METAL
// FIXME
#else
- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect {
    iosBridge::ptr()->onFrame();
}
#endif
@end

#if ORYOL_METAL
@implementation oryolMTKView
#else
@implementation oryolGLKView
#endif
@synthesize touchDelegate;

- (void) setTouchDelegate:(id<touchDelegate>)dlg {
    touchDelegate = dlg;
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
    if (nil != touchDelegate) {
        [touchDelegate touchesBegan:touches withEvent:event];
    }
    else {
        [super touchesBegan:touches withEvent:event];
    }
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
    if (nil != touchDelegate) {
        [touchDelegate touchesMoved:touches withEvent:event];
    }
    else {
        [super touchesMoved:touches withEvent:event];
    }
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
    if (nil != touchDelegate) {
        [touchDelegate touchesEnded:touches withEvent:event];
    }
    else {
        [super touchesEnded:touches withEvent:event];
    }
}

- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    if (nil != touchDelegate) {
        [touchDelegate touchesCancelled:touches withEvent:event];
    }
    else {
        [super touchesCancelled:touches withEvent:event];
    }
}
@end

#if ORYOL_METAL
@interface oryolViewDelegate<MTKViewDelegate> : NSObject
@end

@implementation oryolViewDelegate
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    // FIXME(?)
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    @autoreleasepool {
        iosBridge::ptr()->onFrame();
    }
}
@end
#endif


namespace Oryol {
namespace _priv {

iosBridge* iosBridge::self = nullptr;

//------------------------------------------------------------------------------
iosBridge::iosBridge() :
app(nullptr)
,appDelegate(nil)
,appWindow(nil)
#if ORYOL_METAL
,mtlDevice(nil)
,mtkViewDelegate(nil)
,mtkView(nil)
#else
,eaglContext(nil)
,glkView(nil)
#endif
,mouseScale(1.0)
{
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
iosBridge::~iosBridge() {
    o_assert(nullptr == this->app);
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
iosBridge*
iosBridge::ptr() {
    o_assert_dbg(nullptr != self);
    return self;
}

//------------------------------------------------------------------------------
void
iosBridge::setup(App* app_) {
    o_assert(nullptr == this->app);
    o_assert(nullptr != app_);
    this->app = app_;
    Log::Info("iosBridge::setup() called.\n");
}


//------------------------------------------------------------------------------
void
iosBridge::discard() {
    o_assert(nullptr != this->app);
    Log::Info("iosBridge::discard() called.\n");

    [this->appWindow invalidate]; this->appWindow = nil;
    [this->appDelegate invalidate]; this->appDelegate = nil;
    #if ORYOL_METAL
    ORYOL_OBJC_RELEASE(this->mtlDevice); this->mtlDevice = nil;
    ORYOL_OBJC_RELEASE(this->mtkView); this->mtkView = nil;
    ORYOL_OBJC_RELEASE(this->mtkViewDelegate); this->mtkViewDelegate = nil;
    #else
    ORYOL_OBJC_RELEASE(this->eaglContext); this->eaglContext = nil;
    ORYOL_OBJC_RELEASE(this->glkView); this->glkView = nil;
    #endif
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

    #if ORYOL_METAL
    // view delegate, MTKView and metal device
    this->mtkViewDelegate = [[oryolViewDelegate alloc] init];
    this->mtlDevice = MTLCreateSystemDefaultDevice();
    this->mtkView = [[oryolMTKView alloc] init];
    [this->mtkView setPreferredFramesPerSecond:60];
    [this->mtkView setDelegate:this->mtkViewDelegate];
    [this->mtkView setDevice:this->mtlDevice];
    [this->mtkView setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [this->mtkView setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
    [this->mtkView setUserInteractionEnabled:YES];
    [this->mtkView setMultipleTouchEnabled:YES];
    [win addSubview:this->mtkView];

    // create view controller
    this->mtkViewController = [[UIViewController<MTKViewDelegate> alloc] init];
    [this->mtkViewController setView:this->mtkView];
    [win setRootViewController:this->mtkViewController];

    #else
    // create GL context and GLKView
    // NOTE: the drawable properties will be overridden later in iosDisplayMgr!
    this->eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (this->eaglContext == nil) {
        this->eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }
    oryolGLKView* _glkView = [[oryolGLKView alloc] initWithFrame:mainScreenBounds];
    _glkView.drawableColorFormat   = GLKViewDrawableColorFormatRGBA8888;
    _glkView.drawableDepthFormat   = GLKViewDrawableDepthFormat24;
    _glkView.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
    _glkView.drawableMultisample   = GLKViewDrawableMultisampleNone;
    _glkView.context = this->eaglContext;
    _glkView.delegate = this->appDelegate;
    _glkView.enableSetNeedsDisplay = NO;
    _glkView.userInteractionEnabled = YES;
    _glkView.multipleTouchEnabled = YES;
    _glkView.contentScaleFactor = 1.0f;     // FIXME: this caused different behaviour than Metal path!!!
    this->glkView = _glkView;
    [win addSubview:_glkView];

    // create a GLKViewController
    GLKViewController* _glkViewController = [[GLKViewController alloc] init];
    _glkViewController.view = _glkView;
    _glkViewController.preferredFramesPerSecond = 60;
    this->glkViewController = _glkViewController;
    win.rootViewController = this->glkViewController;
    #endif

    // make window visible
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
iosBridge::onFrame() {
    this->app->onFrame();
}

} // namespace _priv
} // namespace Oryol
