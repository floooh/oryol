#pragma once
//------------------------------------------------------------------------------
/**
    @class Render::DisplaySetup
    @brief display setup parameters
    
    Create a DisplaySetup object and hand it to RenderFacade::SetupDisplay()
    to setup the app window, default render target, and rendering API
    context.
    
    @see RenderFacade, DisplayAttrs
*/
#include "Render/DisplayAttrs.h"

namespace Oryol {
namespace Render {
    
class DisplaySetup : public DisplayAttrs {
public:
    /// shortcut for windowed mode (with RGB8, 24+8 stencil/depth)
    static DisplaySetup Windowed(int32 x, int32 y, int32 w, int32 h, const Core::String& windowTitle);
    /// shortcut for fullscreen mode (with RGB8, 24+8 stencil/depth)
    static DisplaySetup Fullscreen(int32 w, int32 h, const Core::String& windowTitle);
    
    /// get DisplayAttrs object initialized to setup values
    DisplayAttrs GetDisplayAttrs() const;
};
    
} // namespace Render
} // namespace Oryol
