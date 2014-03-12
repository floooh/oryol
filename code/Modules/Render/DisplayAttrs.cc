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
width(0),
height(0),
left(0),
top(0),
colorPixelFormat(PixelFormat::InvalidPixelFormat),
depthPixelFormat(PixelFormat::InvalidPixelFormat),
fullscreen(false) {
    // empty
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetWidth(int32 w) {
    o_assert(w > 0);
    this->width = w;
}

//------------------------------------------------------------------------------
int32
DisplayAttrs::GetWidth() const {
    return this->width;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetHeight(int32 h) {
    o_assert(h > 0);
    this->height = h;
}
    
//------------------------------------------------------------------------------
int32
DisplayAttrs::GetHeight() const {
    return this->height;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetLeft(int32 x) {
    this->left = x;
}
    
//------------------------------------------------------------------------------
int32
DisplayAttrs::GetLeft() const {
    return this->left;
}
    
//------------------------------------------------------------------------------
void
DisplayAttrs::SetTop(int32 y) {
    this->top = y;
}
    
//------------------------------------------------------------------------------
int32
DisplayAttrs::GetTop() const {
    return this->top;
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
DisplayAttrs::SetWindowTitle(const String& title) {
    this->windowTitle = title;
}
    
//------------------------------------------------------------------------------
const String&
DisplayAttrs::GetWindowTitle() const {
    return this->windowTitle;
}

//------------------------------------------------------------------------------
void
DisplayAttrs::SetIconName(const String& icon) {
    this->iconName = icon;
}

//------------------------------------------------------------------------------
const String&
DisplayAttrs::GetIconName() const {
    return this->iconName;
}
    
} // namespace Render
} // namespace Oryol