#pragma once
//------------------------------------------------------------------------------
#include "Gfx/private/resourceBase.h"
#include "Gfx/GfxConfig.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/GfxTypes.h"
#include "Gfx/private/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Mesh
    @ingroup _priv
    @brief D3D11 implementation of mesh
*/
class d3d11Mesh : public meshBase {
public:
    /// destructor
    ~d3d11Mesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    ID3D11Buffer* d3d11VertexBuffer = nullptr;
    ID3D11Buffer* d3d11IndexBuffer = nullptr;
    int vbUpdateFrameIndex = -1;
    int ibUpdateFrameIndex = -1;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Pipeline
    @ingroup _priv
    @brief D3D11 implementation of pipeline
*/
class d3d11Pipeline : public pipelineBase {
public:
    /// constructor
    d3d11Pipeline();
    /// destructor
    ~d3d11Pipeline();

    /// clear the object (called from pipelineFactory::DestroyResource())
    void Clear();

    /// pointer to input layout object
    ID3D11InputLayout* d3d11InputLayout;
    /// rasterize state object
    ID3D11RasterizerState* d3d11RasterizerState;
    /// primitive topology
    D3D_PRIMITIVE_TOPOLOGY d3d11PrimTopology;
    /// depth-stencil state object
    ID3D11DepthStencilState* d3d11DepthStencilState;
    /// blend state object
    ID3D11BlendState* d3d11BlendState;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Shader
    @ingroup _priv
    @brief D3D11 implementation of shader
*/
class d3d11Shader : public shaderBase {
public:
    /// constructor
    d3d11Shader();
    /// destructor
    ~d3d11Shader();

    /// clear the object
    void Clear();

    /// the D3D11 vertex shader
    ID3D11VertexShader* d3d11VertexShader;
    /// the D3D1 pixel shader
    ID3D11PixelShader* d3d11PixelShader;
    /// add a uniform block entry
    void addUniformBlockEntry(ShaderStage::Code bindStage, int bindSlot, ID3D11Buffer* cb);
    /// get uniform block constant buffer at bind stage and slot (can return nullptr)
    ID3D11Buffer* getConstantBuffer(ShaderStage::Code bindStage, int bindSlot) const;

    static const int NumConstantBuffers = ShaderStage::NumShaderStages * GfxConfig::MaxNumUniformBlocksPerStage;
    StaticArray<ID3D11Buffer*, NumConstantBuffers> constantBuffers;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Texture
    @ingroup _priv
    @brief D3D11 implementation of texture
*/
class d3d11Texture : public textureBase {
public:
    /// destructor
    ~d3d11Texture();

    /// clear the object
    void Clear();

    /// d3d11 2D texture object
    ID3D11Texture2D* d3d11Texture2D = nullptr;
    /// d3d11 3D texture object
    ID3D11Texture3D* d3d11Texture3D = nullptr;
    /// d3d11 shader resource view object
    ID3D11ShaderResourceView* d3d11ShaderResourceView = nullptr;
    /// d3d11 sampler state object
    ID3D11SamplerState* d3d11SamplerState = nullptr;
    /// d3d11 depth-stencil texture (if render target with depth buffer)
    ID3D11Texture2D* d3d11DepthStencilTexture = nullptr;
    /// d3d11 MSAA texture (optional)
    ID3D11Texture2D* d3d11MSAATexture2D = nullptr;
    /// d3d11 color texture format
    DXGI_FORMAT d3d11ColorFormat = (DXGI_FORMAT) 0;   // DXGI_FORMAT_UNKNOWN
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11RenderPass
    @ingroup _priv
    @brief D3D11 implementation of renderPass
*/
class d3d11RenderPass : public renderPassBase {
public:
    /// constructor
    d3d11RenderPass();
    /// destructor
    ~d3d11RenderPass();

    /// clear the object
    void Clear();

    /// the d3d11 render-target-view objects
    StaticArray<ID3D11RenderTargetView*, GfxConfig::MaxNumColorAttachments> d3d11RenderTargetViews;
    /// the optional depth-stencil view object
    ID3D11DepthStencilView* d3d11DepthStencilView = nullptr;
};

} // namespace Oryol
} // namespace _priv
