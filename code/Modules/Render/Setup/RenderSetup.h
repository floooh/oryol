#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RenderSetup
    @brief Render module setup parameters
    
    The Render setup object holds the parameter used to setup the
    Rendering system. Create a RenderSetup object, optionally tweak
    its values, and call the RenderFacade::Setup method with the
    RenderSetup object as argument.
 
    @see RenderFacade, DisplayAttrs
*/
#include "Render/Attrs/DisplayAttrs.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class RenderSetup : public DisplayAttrs {
public:
    /// shortcut for windowed mode (with RGB8, 24+8 stencil/depth)
    static RenderSetup Windowed(int32 width, int32 height, Core::String windowTitle="Oryol", PixelFormat::Code colorFmt=PixelFormat::R8G8B8, PixelFormat::Code depthFmt=PixelFormat::D24S8);
    /// shortcut for fullscreen mode (with RGB8, 24+8 stencil/depth)
    static RenderSetup Fullscreen(int32 width, int32 height, Core::String windowTitle="Oryol", PixelFormat::Code colorFmt=PixelFormat::R8G8B8, PixelFormat::Code depthFmt=PixelFormat::D24S8);
    
    /// default constructor
    RenderSetup();
    
    /// tweak resource pool size for a rendering resource type
    void SetPoolSize(ResourceType::Code type, int32 poolSize);
    /// get resource pool size for a rendering resource type
    int32 PoolSize(ResourceType::Code type) const;
    /// tweak resource throttling value for a resource type, 0 means unthrottled
    void SetThrottling(ResourceType::Code type, int32 maxCreatePerFrame);
    /// get resource throttling value
    int32 Throttling(ResourceType::Code type) const;
    /// tweak the resource registry initial capacity (this can reduce memory re-allocations)
    void SetResourceRegistryCapacity(int32 capacity);
    /// get the resource registry initial capacity
    int32 ResourceRegistryCapacity() const;

    /// get DisplayAttrs object initialized to setup values
    DisplayAttrs GetDisplayAttrs() const;

private:
    static const int32 DefaultPoolSize = 128;
    
    int32 poolSizes[ResourceType::NumResourceTypes];
    int32 throttling[ResourceType::NumResourceTypes];
    int32 registryCapacity;
};
    
} // namespace Render
} // namespace Oryol
