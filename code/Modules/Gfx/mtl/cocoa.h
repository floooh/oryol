#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::cocoa
    @ingroup _priv
    @brief cocoa app and window management wrapper
    
    NOTE: this is largely ripped from GLFW3 (https://github.com/glfw/glfw)
*/
#include "Gfx/mtl/mtl_decl.h"
#include "Gfx/Setup/GfxSetup.h"

namespace Oryol {
namespace _priv {

class cocoa {
public:
    /// initialize cocoa wrapper
    void init();
    /// terminate cocoa wrapper
    void terminate();
    /// create app window
    void createWindow(const GfxSetup& setup);
    /// destroy app window
    void destroyWindow();
    /// called by window delegate when window should be closed
    void onWindowShouldClose();
    /// return whether window system wants application to quit
    bool windowShouldClose() const;

    /// pointer to metal device
    ORYOL_OBJC_TYPED_ID(MTLDevice) mtlDevice;
    /// pointer to CAMetalLayer
    CAMetalLayer* mtlLayer = nullptr;

    /// per-window data
    struct cocoaWindowNS {
        ORYOL_OBJC_ID object;
        ORYOL_OBJC_ID delegate;
        ORYOL_OBJC_ID view ;
        unsigned int modifierFlags = 0;
        // The total sum of the distances the cursor has been warped
        // since the last cursor motion event was processed
        // This is kept to counteract Cocoa doing the same internally
        double warpDeltaX = 0.0, warpDeltaY = 0.0;
        bool shouldClose = false;
    } window;

    /// global data
    struct cocoaGlobalNS {
        short int publicKeys[256];
    } global;
};

} // namespace _priv
} // namespace Oryol
