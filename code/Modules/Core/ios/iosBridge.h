#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::iosBridge
    @brief private bridge class between IOS and Oryol app
*/
#include "Core/Types.h"
#include "Core/Macros.h"

#if defined(__OBJC__)
static_assert(sizeof(void*) == sizeof(id), "sizeof(void*) doesn't match sizeof(id)!");
#else
typedef void *id;
#endif

namespace Oryol {
namespace Core {
    
class App;
    
class iosBridge {
    OryolGlobalSingletonDecl(iosBridge);
public:
    /// constructor
    iosBridge(App* app);
    /// destructor
    ~iosBridge();
    
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
    App* app;
    id appDelegate;
    id appWindow;
    id eaglContext;
    id glkView;
    id glkViewController;
};
    
} // namespace Core
} // namespace Oryol