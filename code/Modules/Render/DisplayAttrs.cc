//------------------------------------------------------------------------------
//  DisplayAttrs.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DisplayAttrs.h"

namespace Oryol {
namespace Render {
    
using namespace Core;
    
//------------------------------------------------------------------------------
DisplayAttrs::DisplayAttrs() :
windowWidth(0),
windowHeight(0),
windowPosX(0),
windowPosY(0),
framebufferWidth(0),
framebufferHeight(0),
colorPixelFormat(PixelFormat::InvalidPixelFormat),
depthPixelFormat(PixelFormat::InvalidPixelFormat),
swapInterval(1),
fullscreen(false) {
    // empty
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetWindowWidth(int32 w) {
    o_assert(w > 0);
    this->windowWidth = w;
}

//------------------------------------------------------------------------------
int32
DisplayAttrs::GetWindowWidth() const {
    return this->windowWidth;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetWindowHeight(int32 h) {
    o_assert(h > 0);
    this->windowHeight = h;
}
    
//------------------------------------------------------------------------------
int32
DisplayAttrs::GetWindowHeight() const {
    return this->windowHeight;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetWindowPosX(int32 x) {
    this->windowPosX = x;
}
    
//------------------------------------------------------------------------------
int32
DisplayAttrs::GetWindowPosX() const {
    return this->windowPosX;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetWindowPosY(int32 y) {
    this->windowPosY = y;
}
    
//------------------------------------------------------------------------------
int32
DisplayAttrs::GetWindowPosY() const {
    return this->windowPosY;
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetFramebufferWidth(int32 w) {
    o_assert(w > 0);
    this->framebufferWidth = w;
}

//------------------------------------------------------------------------------
int32
DisplayAttrs::GetFramebufferWidth() const {
    return this->framebufferWidth;
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetFramebufferHeight(int32 h) {
    o_assert(h > 0);
    this->framebufferHeight = h;
}

//------------------------------------------------------------------------------
int32
DisplayAttrs::GetFramebufferHeight() const {
    return this->framebufferHeight;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetColorPixelFormat(PixelFormat::Code pixelFormat) {
    o_assert(PixelFormat::IsValidRenderTargetColorFormat(pixelFormat));
    this->colorPixelFormat = pixelFormat;
}
    
//------------------------------------------------------------------------------
PixelFormat::Code
DisplayAttrs::GetColorPixelFormat() const {
    return this->colorPixelFormat;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetDepthPixelFormat(PixelFormat::Code pixelFormat) {
    o_assert(PixelFormat::IsValidRenderTargetDepthFormat(pixelFormat));
    this->depthPixelFormat = pixelFormat;
}
    
//------------------------------------------------------------------------------
PixelFormat::Code
DisplayAttrs::GetDepthPixelFormat() const {
    return this->depthPixelFormat;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetFullscreen(bool b) {
    this->fullscreen = b;
}
    
//------------------------------------------------------------------------------
bool
DisplayAttrs::IsFullscreen() const {
    return this->fullscreen;
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetSwapInterval(int32 interval) {
    this->swapInterval = interval;
}

//------------------------------------------------------------------------------
int32
DisplayAttrs::GetSwapInterval() const {
    return this->swapInterval;
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetWindowTitle(const String& title) {
    this->windowTitle = title;
}
    
//------------------------------------------------------------------------------
const String&
DisplayAttrs::GetWindowTitle() const {
    return this->windowTitle;
}

} // namespace Render
} // namespace Oryol