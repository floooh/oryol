//------------------------------------------------------------------------------
//  GfxSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxSetup.h"
#include "Core/Assertion.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
GfxSetup::GfxSetup() {
    for (int32 i = 0; i < GfxResourceType::NumResourceTypes; i++) {
        this->poolSizes[i] = DefaultPoolSize;
        this->throttling[i] = 0;    // unthrottled
    }
}

//------------------------------------------------------------------------------
GfxSetup
GfxSetup::Window(int32 w, int32 h, String title) {
    o_assert((w > 0) && (h > 0));

    GfxSetup setup;
    setup.Width    = w;
    setup.Height   = h;
    setup.Windowed = true;
    setup.Title    = title;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup
GfxSetup::Fullscreen(int32 w, int32 h, String title) {
    o_assert((w > 0) && (h > 0));
    
    GfxSetup setup;
    setup.Width    = w;
    setup.Height   = h;
    setup.Windowed = false;
    setup.Title    = title;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup
GfxSetup::WindowMSAA4(int32 w, int32 h, String title) {
    GfxSetup setup = Window(w, h, title);
    setup.Samples = 4;
    return setup;
}

//------------------------------------------------------------------------------
GfxSetup
GfxSetup::FullscreenMSAA4(int32 w, int32 h, String title) {
    GfxSetup setup = Fullscreen(w, h, title);
    setup.Samples = 4;
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
GfxSetup::SetPoolSize(GfxResourceType::Code type, int32 size) {
    o_assert_range(type, GfxResourceType::NumResourceTypes);
    o_assert(size > 0);
    this->poolSizes[type] = size;
}
    
//------------------------------------------------------------------------------
int32
GfxSetup::PoolSize(GfxResourceType::Code type) const {
    o_assert_range(type, GfxResourceType::NumResourceTypes);
    return this->poolSizes[type];
}
    
//------------------------------------------------------------------------------
void
GfxSetup::SetThrottling(GfxResourceType::Code type, int32 maxCreatePerFrame) {
    o_assert_range(type, GfxResourceType::NumResourceTypes);
    this->throttling[type] = maxCreatePerFrame;
}
    
//------------------------------------------------------------------------------
int32
GfxSetup::Throttling(GfxResourceType::Code type) const {
    o_assert_range(type, GfxResourceType::NumResourceTypes);
    return this->throttling[type];
}
    
} // namespace Oryol
