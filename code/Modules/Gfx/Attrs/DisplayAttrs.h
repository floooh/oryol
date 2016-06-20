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
#include "Gfx/Attrs/TextureAttrs.h"

namespace Oryol {

struct DisplayAttrs {
    /// window width (including window chrome)
    int WindowWidth = 0;
    /// window height (including window chrome)
    int WindowHeight = 0;
    /// x-position of window
    int WindowPosX = 0;
    /// y-position of window
    int WindowPosY = 0;
    /// width of framebuffer associated with window
    int FramebufferWidth = 0;
    /// height of framebuffer associated with window
    int FramebufferHeight = 0;
    /// framebuffer pixel format
    PixelFormat::Code ColorPixelFormat = PixelFormat::RGBA8;
    /// depth buffer pixel format (PixelFormat::None if no depth buffer)
    PixelFormat::Code DepthPixelFormat = PixelFormat::DEPTHSTENCIL;
    /// number of multisample-anti-aliasing samples
    int SampleCount = 1;
    /// indicates windowed or fullscreen mode
    bool Windowed = true;
    /// vsync swap interval (0 means: no vsync)
    int SwapInterval = 1;
    /// window title as UTF-8
    String WindowTitle;

    /// init a DisplayAttrs object from a TextureAttrs object
    static DisplayAttrs FromTextureAttrs(const TextureAttrs& texAttrs) {
        DisplayAttrs dispAttrs;
        dispAttrs.WindowWidth = texAttrs.Width;
        dispAttrs.WindowHeight = texAttrs.Height;
        dispAttrs.WindowPosX = 0;
        dispAttrs.WindowPosY = 0;
        dispAttrs.FramebufferWidth = texAttrs.Width;
        dispAttrs.FramebufferHeight = texAttrs.Height;
        dispAttrs.ColorPixelFormat = texAttrs.ColorFormat;
        dispAttrs.DepthPixelFormat = texAttrs.DepthFormat;
        dispAttrs.SampleCount = 1;
        dispAttrs.Windowed = false;
        dispAttrs.SwapInterval = 1;
        return dispAttrs;
    };
};
} // namespace Oryol
 