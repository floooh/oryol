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
#include "Gfx/Core/PassState.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Attrs/DisplayAttrs.h"
#include "Gfx/Attrs/ImageDataAttrs.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Resource/resource.h"
#include "glm/vec4.hpp"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/mtl/mtlReleaseQueue.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

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
    /// get the current render pass attributes
    const DisplayAttrs& renderPassAttrs() const;

    /// begin rendering pass (both ptrs can be nullptr)
    void beginPass(renderPass* pass, const PassState* passState);
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
    /// apply a texture block
    void applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures);

    /// submit a draw call with primitive group index in current mesh
    void draw(int primGroupIndex, int numInstances);
    /// submit a draw call with direct primitive group
    void draw(int baseElementIndex, int numElements, int numInstances);

    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int numBytes);
    /// update texture data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void readPixels(void* buf, int bufNumBytes);

    /// defered-release a render resource
    void releaseDeferred(ORYOL_OBJC_ID obj);
    /// check if command buffer exists, create if not
    void checkCreateCommandBuffer();

    #if ORYOL_MACOS
    static const int MtlUniformAlignment = 256;
    #else
    static const int MtlUniformAlignment = 16;
    #endif

    bool valid;
    GfxSetup gfxSetup;
    gfxPointers pointers;

    int frameIndex;
    int curFrameRotateIndex;

    bool rpValid;
    DisplayAttrs rpAttrs;
    
    pipeline* curPipeline;
    mesh* curPrimaryMesh;
    unsigned long curMTLPrimitiveType;
    unsigned long curMTLIndexType;

    ORYOL_OBJC_TYPED_ID(MTLDevice) mtlDevice;
    ORYOL_OBJC_TYPED_ID(MTLCommandQueue) commandQueue;
    ORYOL_OBJC_TYPED_ID(MTLCommandBuffer) curCommandBuffer;
    ORYOL_OBJC_TYPED_ID(MTLRenderCommandEncoder) curRenderCmdEncoder;

    // rotated global uniform buffers
    uint8_t* curUniformBufferPtr;
    int curUniformBufferOffset;
    StaticArray<ORYOL_OBJC_TYPED_ID(MTLBuffer), GfxConfig::MaxInflightFrames> uniformBuffers;

    // deferred-release-queue, release gfx resources when no longer in use by GPU
    mtlReleaseQueue releaseQueue;
};

} // namespace _priv
} // namespace Oryol
