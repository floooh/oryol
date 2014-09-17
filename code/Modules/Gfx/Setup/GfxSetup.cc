//------------------------------------------------------------------------------
//  GfxSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxSetup.h"
#include "Core/Assert.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
GfxSetup::GfxSetup() :
registryCapacity(1024) {
    for (int32 i = 0; i < ResourceType::NumResourceTypes; i++) {
        this->poolSizes[i] = DefaultPoolSize;
        this->throttling[i] = 0;    // unthrottled
    }
}

//------------------------------------------------------------------------------
GfxSetup
GfxSetup::Window(int32 w, int32 h, bool msaa, String title) {
    o_assert((w > 0) && (h > 0));

    GfxSetup setup;
    setup.Width    = w;
    setup.Height   = h;
    setup.Samples  = msaa ? 4 : 0;
    setup.Windowed = true;
    setup.Title    = title;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup
GfxSetup::Fullscreen(int32 w, int32 h, bool msaa, String title) {
    o_assert((w > 0) && (h > 0));
    
    GfxSetup setup;
    setup.Width    = w;
    setup.Height   = h;
    setup.Samples  = msaa ? 4 : 0;
    setup.Windowed = false;
    setup.Title    = title;
    return setup;
}

//------------------------------------------------------------------------------
DisplayAttrs
GfxSetup::GetDisplayAttrs() const {
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
    attrs.Windowed          = this->Windowed;
    attrs.WindowTitle       = this->Title;
    return attrs;
}

//------------------------------------------------------------------------------
void
GfxSetup::SetPoolSize(ResourceType::Code type, int32 size) {
    o_assert_range(type, ResourceType::NumResourceTypes);
    o_assert(size > 0);
    this->poolSizes[type] = size;
}
    
//------------------------------------------------------------------------------
int32
GfxSetup::PoolSize(ResourceType::Code type) const {
    o_assert_range(type, ResourceType::NumResourceTypes);
    return this->poolSizes[type];
}
    
//------------------------------------------------------------------------------
void
GfxSetup::SetThrottling(ResourceType::Code type, int32 maxCreatePerFrame) {
    o_assert_range(type, ResourceType::NumResourceTypes);
    this->throttling[type] = maxCreatePerFrame;
}
    
//------------------------------------------------------------------------------
int32
GfxSetup::Throttling(ResourceType::Code type) const {
    o_assert_range(type, ResourceType::NumResourceTypes);
    return this->throttling[type];
}
    
//------------------------------------------------------------------------------
void
GfxSetup::SetResourceRegistryCapacity(int32 capacity) {
    o_assert(capacity > 0);
    this->registryCapacity = capacity;
}
    
//------------------------------------------------------------------------------
int32
GfxSetup::ResourceRegistryCapacity() const {
    return this->registryCapacity;
}

} // namespace Oryol
