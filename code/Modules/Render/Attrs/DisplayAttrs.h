#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DisplayAttrs
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Render/Types/PixelFormat.h"

namespace Oryol {
namespace Render {

class DisplayAttrs {
public:
    /// default constructor
    DisplayAttrs();
    
    /// set display width in pixels
    void SetWindowWidth(int32 w);
    /// get display width in pixels
    int32 GetWindowWidth() const;
    /// set display height in pixels
    void SetWindowHeight(int32 h);
    /// get display height in pixels
    int32 GetWindowHeight() const;
    /// set x-pos (ignored if fullscreen)
    void SetWindowPosX(int32 left);
    /// get x-pos
    int32 GetWindowPosX() const;
    /// set y-pos (ignored if fullscreen)
    void SetWindowPosY(int32 top);
    /// get y-pos
    int32 GetWindowPosY() const;
    /// set framebuffer width (usually only set by the display manager)
    void SetFramebufferWidth(int32 w);
    /// get framebuffer width
    int32 GetFramebufferWidth() const;
    /// set framebuffer height (usually only set by the display manager)
    void SetFramebufferHeight(int32 h);
    /// get framebuffer height
    int32 GetFramebufferHeight() const;
    /// set the color pixel format
    void SetColorPixelFormat(PixelFormat::Code colorPixelFormat);
    /// get the color pixel format
    PixelFormat::Code GetColorPixelFormat() const;
    /// set the depth pixel format
    void SetDepthPixelFormat(PixelFormat::Code depthPixelFormat);
    /// get the depth pixel format
    PixelFormat::Code GetDepthPixelFormat() const;
    /// set fullscreen-mode
    void SetFullscreen(bool fullscreen);
    /// get fullscreen flag
    bool IsFullscreen() const;
    /// intended swap interval (0: no vsync, 1: vsync@60fps, etc...)
    void SetSwapInterval(int32 interval);
    /// get swap interval
    int32 GetSwapInterval() const;
    /// set window title (UTF-8)
    void SetWindowTitle(const Core::String& title);
    /// get window title
    const Core::String& GetWindowTitle() const;

protected:
    int32 windowWidth;
    int32 windowHeight;
    int32 windowPosX;
    int32 windowPosY;
    int32 framebufferWidth;
    int32 framebufferHeight;
    PixelFormat::Code colorPixelFormat;
    PixelFormat::Code depthPixelFormat;
    int32 swapInterval;
    bool fullscreen;
    Core::String windowTitle;
};
} // namespace Render
} // namespace Oryol
 