//------------------------------------------------------------------------------
//  d3d12Resource.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Resource.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//==============================================================================
d3d12Mesh::~d3d12Mesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (int i = 0; i < NumSlots; i++) {
            o_assert_dbg(nullptr == buf.d3d12RenderBuffers[i]);
            o_assert_dbg(nullptr == buf.d3d12UploadBuffers[i]);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
d3d12Mesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

//==============================================================================
d3d12Pipeline::d3d12Pipeline() :
    d3d12PipelineState(nullptr),
    d3d12PrimTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED),
    d3d12PrimTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED) {
    // empty
}

//------------------------------------------------------------------------------
d3d12Pipeline::~d3d12Pipeline() {
    o_assert_dbg(nullptr == this->d3d12PipelineState);
}

//------------------------------------------------------------------------------
void
d3d12Pipeline::Clear() {
    this->d3d12PipelineState = nullptr;
    this->d3d12PrimTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    this->d3d12PrimTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    pipelineBase::Clear();
}

//==============================================================================
d3d12Shader::~d3d12Shader() {
    o_assert_dbg(nullptr == this->vertexShader.ptr);
    o_assert_dbg(nullptr == this->pixelShader.ptr);
}

//------------------------------------------------------------------------------
void
d3d12Shader::Clear() {
    this->vertexShader = shaderBlob();
    this->pixelShader = shaderBlob();
    shaderBase::Clear();
}

//==============================================================================
d3d12Texture::d3d12Texture() :
    updateFrameIndex(InvalidIndex),
    numSlots(1),
    activeSlot(0),
    d3d12DepthStencilTextureRes(nullptr),
    d3d12DepthStencilTextureState(D3D12_RESOURCE_STATE_COMMON) {
    for (int i = 0; i < NumSlots; i++) {
        this->slots[i].d3d12TextureRes = nullptr;
        this->slots[i].d3d12UploadBuffer = nullptr;
        this->slots[i].d3d12TextureState = D3D12_RESOURCE_STATE_COMMON;
    }
}

//------------------------------------------------------------------------------
d3d12Texture::~d3d12Texture() {
    o_assert_dbg(nullptr == this->d3d12DepthStencilTextureRes);
#if ORYOL_DEBUG
    for (int i = 0; i < NumSlots; i++) {
        o_assert(nullptr == this->slots[i].d3d12TextureRes);
        o_assert(nullptr == this->slots[i].d3d12UploadBuffer);
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
        this->slots[i].d3d12UploadBuffer = nullptr;
        this->slots[i].d3d12TextureState = D3D12_RESOURCE_STATE_COMMON;
    }
    this->d3d12DepthStencilTextureRes = nullptr;
    this->d3d12DepthStencilTextureState = D3D12_RESOURCE_STATE_COMMON;
}

//==============================================================================
d3d12RenderPass::d3d12RenderPass() {
    this->rtvDescSlots.Fill(InvalidIndex);
    this->dsvDescSlot = InvalidIndex;
}

//------------------------------------------------------------------------------
d3d12RenderPass::~d3d12RenderPass() {
    #if ORYOL_DEBUG
    o_assert(InvalidIndex == this->dsvDescSlot);
    for (int rtv : this->rtvDescSlots) {
        o_assert(InvalidIndex == rtv);
    }
    #endif
}

//------------------------------------------------------------------------------
void
d3d12RenderPass::Clear() {
    renderPassBase::Clear();
    this->rtvDescSlots.Fill(InvalidIndex);
    this->dsvDescSlot = InvalidIndex;
}

} // namespace _priv
} // namespace Oryol
