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
d3d12DepthBufferRes(nullptr),
rtvDescriptorSlot(InvalidIndex),
dsvDescriptorSlot(InvalidIndex) {
    // empty
}

//------------------------------------------------------------------------------
d3d12Texture::~d3d12Texture() {
    o_assert_dbg(nullptr == this->d3d12TextureRes);
    o_assert_dbg(nullptr == this->d3d12DepthBufferRes);
    o_assert_dbg(InvalidIndex == this->rtvDescriptorSlot);
    o_assert_dbg(InvalidIndex == this->dsvDescriptorSlot);
}

//------------------------------------------------------------------------------
void
d3d12Texture::Clear() {
    textureBase::Clear();
    this->d3d12TextureRes = nullptr;
    this->d3d12DepthBufferRes = nullptr;
    this->rtvDescriptorSlot = InvalidIndex;
    this->dsvDescriptorSlot = InvalidIndex;
}

} // namespace _priv
} // namespace Oryol
