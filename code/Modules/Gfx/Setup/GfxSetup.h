#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxSetup
    @ingroup Gfx
    @brief Gfx module setup parameters
    
    The GfxSetup object holds the parameter used to setup the
    rendering system. Create a GfxSetup object, optionally tweak
    its values, and call the Gfx::Setup method with the
    RenderSetup object as argument.
 
    @see Gfx, DisplayAttrs
*/
#include "Core/Containers/Array.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Attrs/DisplayAttrs.h"

namespace Oryol {
    
class GfxSetup {
public:
    /// shortcut for windowed mode (with RGB8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Window(int32 width, int32 height, String windowTitle);
    /// shortcut for fullscreen mode (with RGB8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Fullscreen(int32 width, int32 height, String windowTitle);
    /// shortcut for windowed mode with 4xMSAA (with RGB8, 24+8 stencil/depth)
    static GfxSetup WindowMSAA4(int32 width, int32 height, String windowTitle);
    /// shortcut for fullscreen mode with 4xMSAA (with RGB8, 24+8 stencil/depth)
    static GfxSetup FullscreenMSAA4(int32 width, int32 height, String windowTitle);
    
    /// canvas width
    int32 Width = 640;
    /// canvas height
    int32 Height = 400;
    /// color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGB8;
    /// depth pixel format
    PixelFormat::Code DepthFormat = PixelFormat::D24S8;
    /// MSAA samples (2, 4, 8... no MSAA: 0)
    int32 Samples = 0;
    /// windowed vs Fullscreen
    bool Windowed = true;
    /// window title
    String Title = "Oryol";
    
    /// tweak resource pool size for a rendering resource type
    void SetPoolSize(ResourceType::Code type, int32 poolSize);
    /// get resource pool size for a rendering resource type
    int32 PoolSize(ResourceType::Code type) const;
    /// tweak resource throttling value for a resource type, 0 means unthrottled
    void SetThrottling(ResourceType::Code type, int32 maxCreatePerFrame);
    /// get resource throttling value
    int32 Throttling(ResourceType::Code type) const;
    
    /// initial resource label stack capacity
    int32 ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int32 ResourceRegistryCapacity = 256;

    /// get DisplayAttrs object initialized to setup values
    DisplayAttrs GetDisplayAttrs() const;

    /// default constructor
    GfxSetup();

private:
    static const int32 DefaultPoolSize = 128;
    
    int32 poolSizes[ResourceType::NumResourceTypes];
    int32 throttling[ResourceType::NumResourceTypes];
};
    
} // namespace Oryol
