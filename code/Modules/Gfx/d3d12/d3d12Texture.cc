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
updateFrameIndex(InvalidIndex),
numSlots(1),
activeSlot(0),
d3d12DepthBufferRes(nullptr),
d3d12DepthBufferState(D3D12_RESOURCE_STATE_COMMON),
rtvDescriptorSlot(InvalidIndex),
dsvDescriptorSlot(InvalidIndex) {
    for (int i = 0; i < NumSlots; i++) {
        this->slots[i].d3d12TextureRes = nullptr;
        this->slots[i].d3d12TextureState = D3D12_RESOURCE_STATE_COMMON;
    }
}

//------------------------------------------------------------------------------
d3d12Texture::~d3d12Texture() {
    o_assert_dbg(nullptr == this->d3d12DepthBufferRes);
    o_assert_dbg(InvalidIndex == this->rtvDescriptorSlot);
    o_assert_dbg(InvalidIndex == this->dsvDescriptorSlot);
    #if ORYOL_DEBUG
    for (int i = 0; i < NumSlots; i++) {
        o_assert(nullptr == this->slots[i].d3d12TextureRes);
    }
    #endif
}

//------------------------------------------------------------------------------
void
d3d12Texture::Clear() {
    textureBase::Clear();
    this->updateFrameIndex = InvalidIndex;
    this->numSlots = 1;
    this->activeSlot = 0;
    for (int i = 0; i < NumSlots; i++) {
        this->slots[i].d3d12TextureRes = nullptr;
        this->slots[i].d3d12TextureState = D3D12_RESOURCE_STATE_COMMON;
    }
    this->d3d12DepthBufferRes = nullptr;
    this->d3d12DepthBufferState = D3D12_RESOURCE_STATE_COMMON;
    this->rtvDescriptorSlot = InvalidIndex;
    this->dsvDescriptorSlot = InvalidIndex;
}

} // namespace _priv
} // namespace Oryol
