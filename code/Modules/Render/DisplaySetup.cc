//------------------------------------------------------------------------------
//  DisplaySetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DisplaySetup.h"
#include "Core/Macros.h"

namespace Oryol {
namespace Render {
    
using namespace Core;
    
//------------------------------------------------------------------------------
DisplaySetup
DisplaySetup::Windowed(int32 x, int32 y, int32 w, int32 h, const Core::String& title) {
    o_assert((w > 0) && (h > 0));

    DisplaySetup setup;
    setup.left = x;
    setup.top = y;
    setup.width = w;
    setup.height = h;
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
    setup.width = w;
    setup.height = h;
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
    attrs.SetWidth(this->width);
    attrs.SetHeight(this->height);
    attrs.SetLeft(this->left);
    attrs.SetTop(this->top);
    attrs.SetColorPixelFormat(this->colorPixelFormat);
    attrs.SetDepthPixelFormat(this->depthPixelFormat);
    attrs.SetFullscreen(this->fullscreen);
    attrs.SetWindowTitle(this->windowTitle);
    attrs.SetIconName(this->iconName);
    return attrs;
}


} // namespace Render
} // namespace Oryol