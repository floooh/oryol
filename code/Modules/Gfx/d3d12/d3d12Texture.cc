//------------------------------------------------------------------------------
//  d3d12Texture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Texture.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Texture::d3d12Texture() :
d3d12TextureRes(nullptr),
d3d12DepthBufferRes(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d12Texture::~d3d12Texture() {
    o_assert_dbg(nullptr == this->d3d12TextureRes);
    o_assert_dbg(nullptr == this->d3d12DepthBufferRes);
    o_assert_dbg(!this->renderTargetView.IsValid());
    o_assert_dbg(!this->depthStencilView.IsValid());
}

//------------------------------------------------------------------------------
void
d3d12Texture::Clear() {
    textureBase::Clear();
    this->d3d12TextureRes = nullptr;
    this->d3d12DepthBufferRes = nullptr;
    this->renderTargetView.Invalidate();
    this->depthStencilView.Invalidate();
}

} // namespace _priv
} // namespace Oryol
