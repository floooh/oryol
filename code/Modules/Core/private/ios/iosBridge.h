#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::iosBridge
    @ingroup _priv
    @brief private bridge class between IOS and Oryol app
*/
#include "Core/Types.h"

#if defined(__OBJC__)
#import <UIKit/UIKit.h>
#if ORYOL_METAL
#import <MetalKit/MetalKit.h>
#else
#import <GLKit/GLKit.h>
#endif

#define ORYOL_OBJC_TYPED_ID(clazz) id<clazz>
#define ORYOL_OBJC_ID id
static_assert(sizeof(void*) == sizeof(id), "sizeof(void*) doesn't match sizeof(id)!");

// a delegate for touch events, and our own GLKView/MTKView which delegates touch events
@protocol touchDelegate
- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;
- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;
- (void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;
@end

#if ORYOL_METAL
@interface oryolMTKView: MTKView
#else
@interface oryolGLKView: GLKView
#endif
@property (nonatomic, retain) id<touchDelegate> touchDelegate;
- (void) setTouchDelegate:(id<touchDelegate>)dlg;
@end
#else // __OBJC__
#define ORYOL_OBJC_TYPED_ID(clazz) void*
#define ORYOL_OBJC_ID void*
typedef void *id;
typedef void* MTLDevice;
typedef void MTKView;
#endif

#if ORYOL_USE_ARC
#define ORYOL_OBJC_RELEASE(obj)
#else
#define ORYOL_OBJC_RELEASE(obj) [obj release]
#endif

namespace Oryol {

class App;
    
namespace _priv {
    
class iosBridge {
public:
    /// constructor
    iosBridge();
    /// destructor
    ~iosBridge();
    /// return ptr to global iosBridge object
    static iosBridge* ptr();

    /// setup the iosBridge object
    void setup(App* app);
    /// discard the iosBridge object
    void discard();
    /// setup IOS app and start the main loop
    void startMainLoop();
    
    /// called by app delegate when app has launched
    void onDidFinishLaunching();
    /// called by app delegate when app is about to enter background
    void onWillResignActive();
    /// called by app delegate when app has entered background
    void onDidEnterBackground();
    /// called by app delegate when app is about to enter foreground
    void onWillEnterForeGround();
    /// called by app delegate when app has become active
    void onDidBecomeActive();
    /// called by app delegate when app is about to be killed
    void onWillTerminate();
    /// called per frame by display link, this will in turn call GLKView
    void onDrawRequested();
    /// called per frame by GLKView
    void onFrame();
    
    static iosBridge* self;
    App* app;
    ORYOL_OBJC_ID appDelegate;
    ORYOL_OBJC_ID appWindow;
    #if ORYOL_METAL
    ORYOL_OBJC_TYPED_ID(MTLDevice) mtlDevice;
    ORYOL_OBJC_ID mtkViewDelegate;
    ORYOL_OBJC_ID mtkViewController;
    ORYOL_OBJC_ID mtkView;
    #else
    ORYOL_OBJC_ID eaglContext;
    ORYOL_OBJC_ID glkView;
    ORYOL_OBJC_ID glkViewController;
    #endif
    // this is just a 'global variable' set by mtlDisplayMgr and read by iosInputMgr
    double mouseScale;
};
    
} // namespace _priv
} // namespace Oryol
