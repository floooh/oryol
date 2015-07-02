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
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#define ORYOL_OBJC_TYPED_ID(clazz) id<clazz>
#define ORYOL_OBJC_ID id
static_assert(sizeof(id) == sizeof(void*), "Obj-C id size doesn't match void* size");
#else
#define ORYOL_OBJC_TYPED_ID(clazz) void*
#define ORYOL_OBJC_ID void*
typedef void* CAMetalLayer;
typedef void* CAMetalDrawable;
typedef void* MTLDevice;
typedef void* MTLCommandQueue;
typedef void* MTLRenderPassDescriptor;
typedef void* MTLRenderCommandEncoder;
typedef void* MTLTexture;
#endif
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
    /// pool system events
    void pollEvents();
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
        ORYOL_OBJC_ID delegate;
        ORYOL_OBJC_ID autoreleasePool;
        short int publicKeys[256];
    } global;
};

} // namespace _priv
} // namespace Oryol
