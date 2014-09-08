//------------------------------------------------------------------------------
//  RenderSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RenderSetup.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {
    
using namespace Core;
    
//------------------------------------------------------------------------------
RenderSetup::RenderSetup() :
registryCapacity(1024) {
    for (int32 i = 0; i < ResourceType::NumResourceTypes; i++) {
        this->poolSizes[i] = DefaultPoolSize;
        this->throttling[i] = 0;    // unthrottled
    }
}

//------------------------------------------------------------------------------
RenderSetup
RenderSetup::AsWindow(int32 w, int32 h, bool msaa, String title) {
    o_assert((w > 0) && (h > 0));

    RenderSetup setup;
    setup.Width       = w;
    setup.Height      = h;
    setup.Samples     = msaa ? 4 : 0;
    setup.Fullscreen  = false;
    setup.Title       = title;
    return setup;
}

//------------------------------------------------------------------------------
RenderSetup
RenderSetup::AsFullscreen(int32 w, int32 h, bool msaa, String title) {
    o_assert((w > 0) && (h > 0));
    
    RenderSetup setup;
    setup.Width       = w;
    setup.Height      = h;
    setup.Samples     = msaa ? 4 : 0;
    setup.Fullscreen  = true;
    setup.Title       = title;
    return setup;
}

//------------------------------------------------------------------------------
DisplayAttrs
RenderSetup::GetDisplayAttrs() const {
    DisplayAttrs attrs;
    attrs.WindowWidth       = this->Width;
    attrs.WindowHeight      = this->Height;
    attrs.WindowPosX        = 0;
    attrs.WindowPosY        = 0;
    attrs.FramebufferWidth  = this->Width;
    attrs.FramebufferHeight = this->Height;
    attrs.ColorPixelFormat  = this->ColorFormat;
    attrs.DepthPixelFormat  = this->DepthFormat;
    attrs.Samples           = this->Samples;
    attrs.IsFullscreen      = this->Fullscreen;
    attrs.WindowTitle       = this->Title;
    return attrs;
}

//------------------------------------------------------------------------------
void
RenderSetup::SetPoolSize(ResourceType::Code type, int32 size) {
    o_assert_range(type, ResourceType::NumResourceTypes);
    o_assert(size > 0);
    this->poolSizes[type] = size;
}
    
//------------------------------------------------------------------------------
int32
RenderSetup::PoolSize(ResourceType::Code type) const {
    o_assert_range(type, ResourceType::NumResourceTypes);
    return this->poolSizes[type];
}
    
//------------------------------------------------------------------------------
void
RenderSetup::SetThrottling(ResourceType::Code type, int32 maxCreatePerFrame) {
    o_assert_range(type, ResourceType::NumResourceTypes);
    this->throttling[type] = maxCreatePerFrame;
}
    
//------------------------------------------------------------------------------
int32
RenderSetup::Throttling(ResourceType::Code type) const {
    o_assert_range(type, ResourceType::NumResourceTypes);
    return this->throttling[type];
}
    
//------------------------------------------------------------------------------
void
RenderSetup::SetResourceRegistryCapacity(int32 capacity) {
    o_assert(capacity > 0);
    this->registryCapacity = capacity;
}
    
//------------------------------------------------------------------------------
int32
RenderSetup::ResourceRegistryCapacity() const {
    return this->registryCapacity;
}

} // namespace Render
} // namespace Oryol
