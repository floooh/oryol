#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::cocoa
    @ingroup _priv
    @brief cocoa app and window management wrapper
    
    NOTE: this is largely ripped from GLFW3 (https://github.com/glfw/glfw)
*/

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#define ORYOL_OBJC_TYPED_ID(clazz) id<clazz>
#define ORYOL_OBJC_ID id
#else
#include <ApplicationServices/ApplicationServices.h>
#define ORYOL_OBJC_TYPED_ID(clazz) void*
#define ORYOL_OBJC_ID void*
#endif

namespace Oryol {
namespace _priv {

class cocoa {
public:
    /// initialize cocoa wrapper
    void init();
    /// terminate cocoa wrapper
    void terminate();
    /// create app window
    void createWindow(int width, int height, const char* title);
    /// destroy app window
    void destroyWindow();
    /// pool system events
    void pollEvents();
    /// called by window delegate when window should be closed
    void onWindowShouldClose();
    /// return whether window system wants application to quit
    bool windowShouldClose() const;

    /// pointer to metal device
    ORYOL_OBJC_TYPED_ID(MTLDevice) metalDevice;

    /// per-window data
    struct cocoaWindowNS {
        ORYOL_OBJC_ID object;
        ORYOL_OBJC_ID delegate;
        ORYOL_OBJC_ID view;
        unsigned int modifierFlags;
        // The total sum of the distances the cursor has been warped
        // since the last cursor motion event was processed
        // This is kept to counteract Cocoa doing the same internally
        double warpDeltaX, warpDeltaY;
        bool shouldClose;
    } window;

    /// global data
    struct cocoaGlobalNS {
        ORYOL_OBJC_ID delegate;
        ORYOL_OBJC_ID autoreleasePool;
        short int publicKeys[256];
    } global;
};

} // namespace _priv
} // namespace Oryol
