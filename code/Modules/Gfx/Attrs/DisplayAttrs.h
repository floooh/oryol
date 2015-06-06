#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DisplayAttrs
    @ingroup Gfx
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

struct DisplayAttrs {
    /// window width (including window chrome)
    int32 WindowWidth{0};
    /// window height (including window chrome)
    int32 WindowHeight{0};
    /// x-position of window
    int32 WindowPosX{0};
    /// y-position of window
    int32 WindowPosY{0};
    /// width of framebuffer associated with window
    int32 FramebufferWidth{0};
    /// height of framebuffer associated with window
    int32 FramebufferHeight{0};
    /// framebuffer pixel format
    PixelFormat::Code ColorPixelFormat{PixelFormat::RGB8};
    /// depth buffer pixel format (PixelFormat::None if no depth buffer)
    PixelFormat::Code DepthPixelFormat{PixelFormat::D24S8};
    /// number of multisample-anti-aliasing samples
    int32 Samples{0};
    /// indicates windowed or fullscreen mode
    bool Windowed{true};
    /// vsync swap interval (0 means: no vsync)
    int32 SwapInterval{1};
    /// window title as UTF-8
    String WindowTitle;
};
} // namespace Oryol
 