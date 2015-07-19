#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::osxAppBridge
    @ingroup _priv
    @brief OSX app wrapper (only used for Metal renderer)
    
    This manages the NSApplication object, window, MTKView and 
    calls the application-on-frame method from the controlled by the MTKView.
*/
#include "Core/Types.h"
#include "Core/Macros.h"

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>
#define ORYOL_OBJC_TYPED_ID(clazz) id<clazz>
#define ORYOL_OBJC_ID id
static_assert(sizeof(void*) == sizeof(id), "sizeof(void*) doesn't match sizeof(id)!");

@interface oryolMTKView: MTKView
@end
#else
#define ORYOL_OBJC_TYPED_ID(clazz) void*
#define ORYOL_OBJC_ID void*
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

class osxAppBridge {
public:
    /// constructor
    osxAppBridge();
    /// destructor
    ~osxAppBridge();
    /// get checked pointer
    static osxAppBridge* ptr();

    /// setup the object
    void setup(App* app);
    /// discard the object
    void discard();
    /// start the main loop (does not return until app quits)
    void startMainLoop();
    /// return true if the window requests app to close
    bool shouldClose();

    /// called from AppState Destroy
    void onDestroy();
    /// called by app delegate when app has launched
    void onDidFinishLaunching();
    /// called by app delagate when application should terminate
    void onShouldTerminate();
    /// called by app delegate when application will terminate
    void onWillTerminate();
    /// called by app delegate when application will become active
    void onWillBecomeActive();
    /// called by app delegate when application did become active
    void onDidBecomeActive();
    /// called by app delegate when application will resign from active state
    void onWillResignActive();
    /// called by app delegate when application did resign from active state
    void onDidResignActive();
    /// called by app delegate when application will hide
    void onWillHide();
    /// called by app delegate when application did hide
    void onDidHide();
    /// called by app delegate when application will unhide
    void onWillUnhide();
    /// called by app delegate when application did unhide
    void onDidUnhide();
    /// called by app delegate when screen parameters have changed
    void onDidChangeScreenParameters();
    /// called by app when occlusion state changes
    void onDidChangeOcclusionState();

    /// called when window receives close request
    void onWindowShouldClose();
    /// called when window did resize
    void onWindowDidResize();
    /// called when window did move
    void onWindowDidMove();
    /// called when window did miniaturize
    void onWindowDidMiniaturize();
    /// called when window did deminiaturize
    void onWindowDidDeminiaturize();
    /// called when window did become key window
    void onWindowDidBecomeKey();
    /// called when window no longer key window
    void onWindowDidResignKey();

    /// called per frame by the MTKView delegate
    void onFrame();

    /// create the application window (hidden)
    void createWindow();
    /// actually show the window (called from displayMgr)
    void showWindow();

    static osxAppBridge* self;
    App* app;
    bool shouldCloseFlag;
    ORYOL_OBJC_ID appDelegate;
    ORYOL_OBJC_ID appWindow;
    ORYOL_OBJC_ID appWindowDelegate;
    ORYOL_OBJC_TYPED_ID(MTLDevice) mtlDevice;
    ORYOL_OBJC_ID mtkViewDelegate;
    MTKView* mtkView;
};

} // namespace _priv
} // namespace Oryol

