//------------------------------------------------------------------------------
//  TextureAttrs.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureAttrs.h"

namespace Oryol {
namespace Render {

using namespace Resource;
    
//------------------------------------------------------------------------------
TextureAttrs::TextureAttrs() :
type(TextureType::InvalidTextureType),
colorFormat(PixelFormat::InvalidPixelFormat),
depthFormat(PixelFormat::InvalidPixelFormat),
usage(Usage::InvalidUsage),
width(0),
height(0),
depth(0),
hasMipmaps(false),
isRenderTarget(false),
hasDepthBuffer(false),
hasSharedDepthBuffer(false),
isDepthTexture(false) {
    // empty
}

//------------------------------------------------------------------------------
void
TextureAttrs::setLocator(const Locator& loc) {
    this->locator = loc;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setType(TextureType::Code t) {
    this->type = t;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setColorFormat(PixelFormat::Code f) {
    this->colorFormat = f;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setDepthFormat(PixelFormat::Code f) {
    this->depthFormat = f;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setUsage(Usage::Code u) {
    this->usage = u;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setWidth(int32 w) {
    this->width = w;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setHeight(int32 h) {
    this->height = h;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setDepth(int32 d) {
    this->depth = d;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setMipmapsFlag(bool b) {
    this->hasMipmaps = b;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setRenderTargetFlag(bool b) {
    this->isRenderTarget = b;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setDepthBufferFlag(bool b) {
    this->hasDepthBuffer = b;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setSharedDepthBufferFlag(bool b) {
    this->hasSharedDepthBuffer = b;
}

//------------------------------------------------------------------------------
void
TextureAttrs::setDepthTextureFlag(bool b) {
    this->isDepthTexture = b;
}

} // namespace Render
} // namespace Oryol