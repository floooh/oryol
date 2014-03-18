//------------------------------------------------------------------------------
//  DisplaySetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DisplaySetup.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {
    
using namespace Core;
    
//------------------------------------------------------------------------------
DisplaySetup
DisplaySetup::Windowed(int32 w, int32 h, const Core::String& title) {
    o_assert((w > 0) && (h > 0));

    DisplaySetup setup;
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
DisplaySetup
DisplaySetup::Fullscreen(int32 w, int32 h, const Core::String& title) {
    o_assert((w > 0) && (h > 0));
    
    DisplaySetup setup;
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
DisplaySetup::GetDisplayAttrs() const {
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


} // namespace Render
} // namespace Oryol