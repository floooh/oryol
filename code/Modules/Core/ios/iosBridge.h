#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::iosBridge
    @ingroup _priv
    @brief private bridge class between IOS and Oryol app
*/
#include "Core/Types.h"
#include "Core/Macros.h"

#if defined(__OBJC__)
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

static_assert(sizeof(void*) == sizeof(id), "sizeof(void*) doesn't match sizeof(id)!");

// a delegate for touch events, and our own GLKView which delegates touch events
@protocol touchDelegate
- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;
- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;
- (void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;
@end

@interface oryolGLKView: GLKView
@property (nonatomic, retain) id<touchDelegate> touchDelegate;
- (void) setTouchDelegate:(id<touchDelegate>)dlg;
@end

#else
typedef void *id;
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
    
    /// get app window
    id iosGetAppWindow() const;
    /// get app delegate
    id iosGetAppDelegate() const;
    /// get our EAGLContext
    id iosGetEAGLContext() const;
    /// get our GLKView
    id iosGetGLKView() const;
    /// get our GLKViewController
    id iosGetGLKViewController() const;
    
private:
    static iosBridge* self;
    App* app;
    id appDelegate;
    id appWindow;
    id eaglContext;
    id glkView;
    id glkViewController;
};
    
} // namespace _priv
} // namespace Oryol