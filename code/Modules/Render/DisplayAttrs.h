#pragma once
//------------------------------------------------------------------------------
/**
    @class Render::DisplayAttrs
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Render/PixelFormat.h"

namespace Oryol {
namespace Render {

class DisplayAttrs {
public:
    /// default constructor
    DisplayAttrs();
    
    /// set display width in pixels
    void SetWidth(int32 w);
    /// get display width in pixels
    int32 GetWidth() const;
    /// set display height in pixels
    void SetHeight(int32 h);
    /// get display height in pixels
    int32 GetHeight() const;
    /// set x-pos (ignored if fullscreen)
    void SetLeft(int32 left);
    /// get x-pos
    int32 GetLeft() const;
    /// set y-pos (ignored if fullscreen)
    void SetTop(int32 top);
    /// get y-pos
    int32 GetTop() const;
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
    /// set window title (UTF-8)
    void SetWindowTitle(const Core::String& title);
    /// get window title
    const Core::String& GetWindowTitle() const;
    /// set icon name
    void SetIconName(const Core::String& icon);
    /// get icon name
    const Core::String& GetIconName() const;

protected:
    int32 width;
    int32 height;
    int32 left;
    int32 top;
    PixelFormat::Code colorPixelFormat;
    PixelFormat::Code depthPixelFormat;
    bool fullscreen;
    Core::String windowTitle;
    Core::String iconName;
};
} // namespace Render
} // namespace Oryol
 