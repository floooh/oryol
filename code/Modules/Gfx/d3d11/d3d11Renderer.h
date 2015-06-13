#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Renderer
    @ingroup _priv
    @brief D3D11 implementation of renderer
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Attrs/DisplayAttrs.h"
#include <glm/vec4.hpp>
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class meshPool;
class texturePool;
class displayMgr;
class texture;
class drawState;
class mesh;
class programBundle;
    
class d3d11Renderer {
public:
    /// constructor
    d3d11Renderer();
    /// destructor
    ~d3d11Renderer();
    
    /// setup the renderer
    void setup(displayMgr* dispMgr, meshPool* mshPool, texturePool* texPool);
    /// discard the renderer
    void discard();
    /// return true if renderer has been setup
    bool isValid() const;
    
    /// reset the internal state cache
    void resetStateCache();
    /// test if a feature is supported
    bool supports(GfxFeature::Code feat) const;
    /// commit current frame
    void commitFrame();
    /// get the current render target attributes
    const DisplayAttrs& renderTargetAttrs() const;

    /// apply a render target (default or offscreen)
    void applyRenderTarget(texture* rt);
    /// apply viewport
    void applyViewPort(int32 x, int32 y, int32 width, int32 height);
    /// apply scissor rect
    void applyScissorRect(int32 x, int32 y, int32 width, int32 height);
    /// apply draw state
    void applyDrawState(drawState* ds);
    /// apply a shader uniform block
    void applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize);
    /// clear currently assigned render target
    void clear(ClearTarget::Mask clearMask, const glm::vec4& color, float32 depth, uint8 stencil);
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
    void readPixels(displayMgr* displayManager, void* buf, int32 bufNumBytes);

    /// invalidate currently bound mesh state
    void invalidateMeshState();
    /// invalidate currently bound shader program state
    void invalidateProgramState();
    /// invalidate currently bound draw state 
    void invalidateDrawState();

    /// pointer to d3d11 device
    ID3D11Device* d3d11Device;
    /// pointer to immediate mode device context
    ID3D11DeviceContext* d3d11DeviceContext;

private:
    bool valid;
    displayMgr* dispMgr;
    meshPool* mshPool;
    texturePool* texPool;
    ID3D11RenderTargetView* defaultRenderTargetView;
    ID3D11DepthStencilView* defaultDepthStencilView;

    bool rtValid;
    DisplayAttrs rtAttrs;
    
    // high-level state cache
    texture* curRenderTarget;
    drawState* curDrawState;

    ID3D11RenderTargetView* curRenderTargetView;
    ID3D11DepthStencilView* curDepthStencilView;
};

} // namespace _priv
} // namespace Oryol
