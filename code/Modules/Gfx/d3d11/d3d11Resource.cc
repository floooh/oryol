//------------------------------------------------------------------------------
//  d3d11Resource.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Resource.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//==============================================================================
d3d11Mesh::~d3d11Mesh() {
    o_assert_dbg(nullptr == this->d3d11VertexBuffer);
    o_assert_dbg(nullptr == this->d3d11IndexBuffer);
}

//------------------------------------------------------------------------------
void
    d3d11Mesh::Clear() {
    this->d3d11VertexBuffer = nullptr;
    this->d3d11IndexBuffer = nullptr;
    this->vbUpdateFrameIndex = -1;
    this->ibUpdateFrameIndex = -1;
    meshBase::Clear();
}

//==============================================================================
d3d11Pipeline::d3d11Pipeline() :
    d3d11InputLayout(nullptr),
    d3d11PrimTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED),
    d3d11RasterizerState(nullptr),
    d3d11DepthStencilState(nullptr),
    d3d11BlendState(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d11Pipeline::~d3d11Pipeline() {
#if ORYOL_DEBUG
    o_assert_dbg(nullptr == this->d3d11InputLayout);
    o_assert_dbg(nullptr == this->d3d11RasterizerState);
    o_assert_dbg(nullptr == this->d3d11DepthStencilState);
    o_assert_dbg(nullptr == this->d3d11BlendState);
#endif
}

//------------------------------------------------------------------------------
void
d3d11Pipeline::Clear() {
    this->d3d11InputLayout = nullptr;
    this->d3d11PrimTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    this->d3d11RasterizerState = nullptr;
    this->d3d11DepthStencilState = nullptr;
    this->d3d11BlendState = nullptr;
    pipelineBase::Clear();
}

//==============================================================================
d3d11Shader::d3d11Shader() {
    this->Clear();
}

//------------------------------------------------------------------------------
d3d11Shader::~d3d11Shader() {
#if ORYOL_DEBUG
    o_assert_dbg(nullptr == this->d3d11VertexShader);
    o_assert_dbg(nullptr == this->d3d11PixelShader);
    for (auto cb : this->constantBuffers) {
        o_assert_dbg(nullptr == cb);
    }
#endif
}

//------------------------------------------------------------------------------
void
d3d11Shader::Clear() {
    this->d3d11VertexShader = nullptr;
    this->d3d11PixelShader = nullptr;
    this->constantBuffers.Fill(nullptr);
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
void
d3d11Shader::addUniformBlockEntry(ShaderStage::Code bindStage, int bindSlot, ID3D11Buffer* cb) {
    o_assert_dbg(cb);
    const int cbIndex = (GfxConfig::MaxNumUniformBlocksPerStage * bindStage) + bindSlot;
    this->constantBuffers[cbIndex] = cb;
}

//------------------------------------------------------------------------------
ID3D11Buffer*
d3d11Shader::getConstantBuffer(ShaderStage::Code bindStage, int bindSlot) const {
    const int cbIndex = (GfxConfig::MaxNumUniformBlocksPerStage * bindStage) + bindSlot;
    return this->constantBuffers[cbIndex];
}

//==============================================================================
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
