#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxSetup
    @ingroup Gfx
    @brief Gfx module setup parameters
    
    The GfxSetup object holds the parameter used to setup the
    rendering system. Create a GfxSetup object, optionally tweak
    its values, and call the Gfx::Setup method with the
    GfxSetup object as argument.
 
    @see Gfx, DisplayAttrs
*/
#include "Core/Containers/Array.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Attrs/DisplayAttrs.h"

namespace Oryol {
    
class GfxSetup {
public:
    /// shortcut for windowed mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Window(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Fullscreen(int width, int height, String windowTitle);
    /// shortcut for windowed mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup WindowMSAA4(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup FullscreenMSAA4(int width, int height, String windowTitle);
    
    /// canvas width
    int Width = 640;
    /// canvas height
    int Height = 400;
    /// color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// depth pixel format
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    /// MSAA samples (2, 4, 8... no MSAA: 1)
    int SampleCount = 1;
    /// windowed vs Fullscreen
    bool Windowed = true;
    /// swap interval (0 => no vsync, default is 1)
    int SwapInterval = 1;
    /// window title
    String Title = "Oryol";
    /// optional clear-hint how the default render target is clear, used in D3D12 for MSAA backbuffer
    ClearState ClearHint;
    /// enable to render full-res on HighDPI displays (not supported on all platforms)
    bool HighDPI = false;
    
    /// tweak resource pool size for a rendering resource type
    void SetPoolSize(GfxResourceType::Code type, int poolSize);
    /// get resource pool size for a rendering resource type
    int PoolSize(GfxResourceType::Code type) const;
    /// tweak resource throttling value for a resource type, 0 means unthrottled
    void SetThrottling(GfxResourceType::Code type, int maxCreatePerFrame);
    /// get resource throttling value
    int Throttling(GfxResourceType::Code type) const;
    
    /// initial resource label stack capacity
    int ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int ResourceRegistryCapacity = 256;
    /// size of the global uniform buffer (only relevant on some platforms)
    int GlobalUniformBufferSize = GfxConfig::DefaultGlobalUniformBufferSize;
    /// max number of drawcalls per frame (only relevant on some platforms)
    int MaxDrawCallsPerFrame = GfxConfig::DefaultMaxDrawCallsPerFrame;
    /// max number of ApplyDrawState per frame (only relevant on some platforms)
    int MaxApplyDrawStatesPerFrame = GfxConfig::DefaultMaxApplyDrawStatesPerFrame;

    /// get DisplayAttrs object initialized to setup values
    DisplayAttrs GetDisplayAttrs() const;

    /// default constructor
    GfxSetup();

private:
    int poolSizes[GfxResourceType::NumResourceTypes];
    int throttling[GfxResourceType::NumResourceTypes];
};
    
} // namespace Oryol
