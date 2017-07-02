#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Renderer
    @ingroup _priv
    @brief D3D11 implementation of renderer
*/
#include "Core/Types.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/GfxTypes.h"
#include <glm/vec4.hpp>
#include "Gfx/private/d3d11/d3d11_decl.h"
#include "Gfx/private/gfxPointers.h"

namespace Oryol {
namespace _priv {

class texture;
class pipeline;
class mesh;
class textureBlock;
class renderPass;
    
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
    /// get the current render pass attributes
    const DisplayAttrs& renderPassAttrs() const;

    /// begin rendering pass (both ptrs can be nullptr)
    void beginPass(renderPass* pass, const PassAction* action);
    /// end current rendering pass
    void endPass();
    /// apply viewport
    void applyViewPort(int x, int y, int width, int height, bool originTopLeft);
    /// apply scissor rect
    void applyScissorRect(int x, int y, int width, int height, bool originTopLeft);
    /// apply draw state
    void applyDrawState(pipeline* pip, mesh** meshes, int numMeshes);
    /// apply a shader uniform block
    void applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize);
    /// apply a textures
    void applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures);
    /// submit a draw call with primitive group index in current mesh
    void draw(int primGroupIndex, int numInstances);
    /// submit a draw call with element range
    void draw(int baseElementIndex, int numElements, int numInstances);
    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int numBytes);
    /// update texture data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes);

    /// invalidate currently bound mesh state
    void invalidateMeshState();
    /// invalidate currently bound shader program state
    void invalidateShaderState();
    /// invalidate currently bound draw state 
    void invalidatePipeline();
    /// invalidate currently bound texture state
    void invalidateTextureState();

    /// pointer to d3d11 device
    ID3D11Device* d3d11Device;
    /// pointer to immediate mode device context
    ID3D11DeviceContext* d3d11DeviceContext;

    bool valid;
    bool rpValid;
    int frameIndex;
    GfxSetup gfxSetup;
    gfxPointers pointers;
    DisplayAttrs rpAttrs;

    renderPass* curRenderPass;
    pipeline* curPipeline;
    mesh* curPrimaryMesh;
    
    int numRTVs;
    StaticArray<ID3D11RenderTargetView*, GfxConfig::MaxNumColorAttachments> d3d11CurRTVs;
    ID3D11DepthStencilView* d3d11CurDSV;
    ID3D11RasterizerState* d3d11CurRasterizerState;
    ID3D11DepthStencilState* d3d11CurDepthStencilState;
    ID3D11BlendState* d3d11CurBlendState;
    ID3D11Buffer* d3d11CurIndexBuffer;
    ID3D11InputLayout* d3d11CurInputLayout;
    ID3D11VertexShader* d3d11CurVertexShader;
    ID3D11PixelShader* d3d11CurPixelShader;
    D3D_PRIMITIVE_TOPOLOGY d3d11CurPrimitiveTopology;
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumUniformBlocksPerStage> d3d11CurVSCBs;
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumUniformBlocksPerStage> d3d11CurPSCBs;
    StaticArray<ID3D11Buffer*, GfxConfig::MaxNumInputMeshes> d3d11CurVBs;
    StaticArray<uint32_t, GfxConfig::MaxNumInputMeshes> curVertexStrides;
    StaticArray<uint32_t, GfxConfig::MaxNumInputMeshes> curVertexOffsets;
    StaticArray<ID3D11ShaderResourceView*, GfxConfig::MaxNumVertexTextures> d3d11CurVSSRVs;
    StaticArray<ID3D11ShaderResourceView*, GfxConfig::MaxNumFragmentTextures> d3d11CurPSSRVs;
    StaticArray<ID3D11SamplerState*, GfxConfig::MaxNumVertexTextures> d3d11CurVSSamplers;
    StaticArray<ID3D11SamplerState*, GfxConfig::MaxNumFragmentTextures> d3d11CurPSSamplers;

    uint16_t curStencilRef;
    glm::vec4 curBlendColor;
};

} // namespace _priv
} // namespace Oryol
