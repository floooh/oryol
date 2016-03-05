//------------------------------------------------------------------------------
//  d3d11Texture.h
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Texture.h"
#include "Gfx/d3d11/d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Texture::~d3d11Texture() {
    o_assert_dbg(nullptr == this->d3d11Texture2D);
    o_assert_dbg(nullptr == this->d3d11ShaderResourceView);
    o_assert_dbg(nullptr == this->d3d11SamplerState);
    o_assert_dbg(nullptr == this->d3d11RenderTargetView);
    o_assert_dbg(nullptr == this->d3d11DepthBufferTexture);
    o_assert_dbg(nullptr == this->d3d11DepthStencilView);
}

//------------------------------------------------------------------------------
void
d3d11Texture::Clear() {
    textureBase::Clear();
    this->d3d11Texture2D = nullptr;
    this->d3d11ShaderResourceView = nullptr;
    this->d3d11SamplerState = nullptr;
    this->d3d11RenderTargetView = nullptr;
    this->d3d11DepthBufferTexture = nullptr;
    this->d3d11DepthStencilView = nullptr;
}

} // namespace _priv
} // namespace Oryol