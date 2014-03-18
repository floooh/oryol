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
RenderSetup::Windowed(int32 w, int32 h, const Core::String& title) {
    o_assert((w > 0) && (h > 0));

    RenderSetup setup;
    setup.windowWidth = w;
    setup.windowHeight = h;
    setup.framebufferWidth = w;
    setup.framebufferHeight = h;
    setup.colorPixelFormat = PixelFormat::R8G8B8;
    setup.depthPixelFormat = PixelFormat::D24S8;
    setup.fullscreen = false;
    setup.windowTitle = title;
    return setup;
}

//------------------------------------------------------------------------------
RenderSetup
RenderSetup::Fullscreen(int32 w, int32 h, const Core::String& title) {
    o_assert((w > 0) && (h > 0));
    
    RenderSetup setup;
    setup.windowWidth = w;
    setup.windowHeight = h;
    setup.framebufferWidth = w;
    setup.framebufferHeight = h;
    setup.colorPixelFormat = PixelFormat::R8G8B8;
    setup.depthPixelFormat = PixelFormat::D24S8;
    setup.fullscreen = true;
    setup.windowTitle = title;
    return setup;
}

//------------------------------------------------------------------------------
DisplayAttrs
RenderSetup::GetDisplayAttrs() const {
    DisplayAttrs attrs;
    attrs.SetWindowWidth(this->windowWidth);
    attrs.SetWindowHeight(this->windowHeight);
    attrs.SetWindowPosX(this->windowPosX);
    attrs.SetWindowPosY(this->windowPosY);
    attrs.SetFramebufferWidth(this->framebufferWidth);
    attrs.SetFramebufferHeight(this->framebufferHeight);
    attrs.SetColorPixelFormat(this->colorPixelFormat);
    attrs.SetDepthPixelFormat(this->depthPixelFormat);
    attrs.SetFullscreen(this->fullscreen);
    attrs.SetWindowTitle(this->windowTitle);
    attrs.SetSwapInterval(this->swapInterval);
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
RenderSetup::GetPoolSize(ResourceType::Code type) const {
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
RenderSetup::GetThrottling(ResourceType::Code type) const {
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
RenderSetup::GetResourceRegistryCapacity() const {
    return this->registryCapacity;
}

} // namespace Render
} // namespace Oryol