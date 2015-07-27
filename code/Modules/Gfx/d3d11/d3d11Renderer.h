#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Renderer
    @ingroup _priv
    @brief D3D11 implementation of renderer
*/
#include "Core/Types.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Attrs/DisplayAttrs.h"
#include <glm/vec4.hpp>
#include "Gfx/d3d11/d3d11_decl.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class texture;
class drawState;
class mesh;
    
class d3d11Renderer {
public:
    /// constructor
    d3d11Renderer();
    /// destructor
    ~d3d11Renderer();
    
    /// setup the renderer
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the renderer
    void discard();
    /// return true if renderer has been setup
    bool isValid() const;
    
    /// reset the internal state cache
    void resetStateCache();
    /// test if a feature is supported
    bool queryFeature(GfxFeature::Code feat) const;
    /// commit current frame
    void commitFrame();
    /// get the current render target attributes
    const DisplayAttrs& renderTargetAttrs() const;

    /// apply a render target (default or offscreen)
    void applyRenderTarget(texture* rt, const ClearState& clearState);
    /// apply viewport
    void applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft);
    /// apply scissor rect
    void applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft);
    /// apply draw state
    void applyDrawState(drawState* ds);
    /// apply a shader uniform block
    void applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize);
    /// submit a draw call with primitive group index in current mesh
    void draw(int32 primGroupIndex);
    /// submit a draw call with direct primitive group
    void draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering with primitive group index in current mesh
    void drawInstanced(int32 primGroupIndex, int32 numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    void drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances);
    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int32 numBytes);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void readPixels(void* buf, int32 bufNumBytes);

    /// invalidate currently bound mesh state
    void invalidateMeshState();
    /// invalidate currently bound shader program state
    void invalidateShaderState();
    /// invalidate currently bound draw state 
    void invalidateDrawState();
    /// invalidate currently bound texture state
    void invalidateTextureState();

    /// pointer to d3d11 device
    ID3D11Device* d3d11Device;
    /// pointer to immediate mode device context
    ID3D11DeviceContext* d3d11DeviceContext;

private:
    bool valid;
    gfxPointers pointers;

    bool rtValid;
    DisplayAttrs rtAttrs;
    
    // high-level state cache
    texture* curRenderTarget;
    drawState* curDrawState;

    ID3D11RenderTargetView* d3d11CurRenderTargetView;
    ID3D11DepthStencilView* d3d11CurDepthStencilView;
    ID3D11RasterizerState* d3d11CurRasterizerState;
    ID3D11DepthStencilState* d3d11CurDepthStencilState;
    ID3D11BlendState* d3d11CurBlendState;
    ID3D11Buffer* d3d11CurIndexBuffer;
    ID3D11InputLayout* d3d11CurInputLayout;
    ID3D11VertexShader* d3d11CurVertexShader;
    ID3D11PixelShader* d3d11CurPixelShader;
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumUniformBlocks> d3d11CurVSConstantBuffers;
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumUniformBlocks> d3d11CurPSConstantBuffers;
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumInputMeshes> d3d11CurVertexBuffers;
    StaticArray<uint32, GfxConfig::MaxNumInputMeshes> curVertexStrides;
    StaticArray<uint32, GfxConfig::MaxNumInputMeshes> curVertexOffsets;
    StaticArray<ID3D11ShaderResourceView*, GfxConfig::MaxNumUniformLayoutComponents> d3d11CurVSShaderResourceViews;
    StaticArray<ID3D11ShaderResourceView*, GfxConfig::MaxNumUniformLayoutComponents> d3d11CurPSShaderResourceViews;
    StaticArray<ID3D11SamplerState*, GfxConfig::MaxNumUniformLayoutComponents> d3d11CurVSSamplerStates;
    StaticArray<ID3D11SamplerState*, GfxConfig::MaxNumUniformLayoutComponents> d3d11CurPSSamplerStates;

    uint16 curStencilRef;
    glm::vec4 curBlendColor;
    PrimitiveType::Code curPrimitiveTopology;
};

} // namespace _priv
} // namespace Oryol
