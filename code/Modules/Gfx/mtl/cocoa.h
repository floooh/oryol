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
#else
#include <ApplicationServices/ApplicationServices.h>
typedef void* id;
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

    /// per-window data
    struct cocoaWindowNS {
        id object;
        id delegate;
        id view;
        unsigned int modifierFlags;
        // The total sum of the distances the cursor has been warped
        // since the last cursor motion event was processed
        // This is kept to counteract Cocoa doing the same internally
        double warpDeltaX, warpDeltaY;
    } window;

    /// global data
    struct cocoaGlobalNS {
        id delegate;
        id autoreleasePool;
        short int publicKeys[256];
    } global;
};




} // namespace _priv
} // namespace Oryol
