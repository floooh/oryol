#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlRenderer
    @ingroup _priv
    @brief Metal implementation of class 'renderer'
*/
#include "Core/Types.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Attrs/DisplayAttrs.h"
#include "Gfx/Attrs/ImageDataAttrs.h"
#include "Gfx/Setup/GfxSetup.h"
#include "glm/vec4.hpp"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/mtl/mtlReleaseQueue.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class texture;
class drawState;
class mesh;
class textureBlock;

class mtlRenderer {
public:
    /// constructor
    mtlRenderer();
    /// destructor
    ~mtlRenderer();
    
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
    void applyUniformBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 layoutHash, const uint8* ptr, int32 byteSize);
    /// apply a texture block
    void applyTextureBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 layoutHash, texture** textures, int32 numTextures);
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
    /// update index data
    void updateIndices(mesh* msh, const void* data, int32 numBytes);
    /// update texture data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void readPixels(void* buf, int32 bufNumBytes);

    /// defered-release a render resource
    void releaseDeferred(ORYOL_OBJC_ID obj);

    bool valid;
    GfxSetup gfxSetup;
    gfxPointers pointers;

    int32 frameIndex;
    int32 curFrameRotateIndex;

    bool rtValid;
    DisplayAttrs rtAttrs;
    
    drawState* curDrawState;
    ORYOL_OBJC_TYPED_ID(MTLDevice) mtlDevice;
    ORYOL_OBJC_TYPED_ID(MTLCommandQueue) commandQueue;
    ORYOL_OBJC_TYPED_ID(MTLCommandBuffer) curCommandBuffer;
    ORYOL_OBJC_TYPED_ID(MTLRenderCommandEncoder) curCommandEncoder;

    // rotated global uniform buffers
    uint8* curUniformBufferPtr;
    int32 curUniformBufferOffset;
    StaticArray<ORYOL_OBJC_TYPED_ID(MTLBuffer), GfxConfig::MtlMaxInflightFrames> uniformBuffers;

    // deferred-release-queue, release gfx resources when no longer in use by GPU
    mtlReleaseQueue releaseQueue;
};

} // namespace _priv
} // namespace Oryol
