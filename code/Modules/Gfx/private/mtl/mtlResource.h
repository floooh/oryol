#pragma once
//------------------------------------------------------------------------------
#include "Gfx/private/resourceBase.h"
#include "Gfx/private/mtl/mtl_decl.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/GfxConfig.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlMesh
    @ingroup _priv
    @brief Metal implementation of class mesh
*/
class mtlMesh : public meshBase {
public:
    /// destructor
    ~mtlMesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    static const int NumSlots = GfxConfig::MaxInflightFrames;
    struct buffer {
        buffer();
        int updateFrameIndex;
        uint8_t numSlots;
        uint8_t activeSlot;
        StaticArray<ORYOL_OBJC_TYPED_ID(MTLBuffer), NumSlots> mtlBuffers;
    };
    // indices into buffers array (first entry is vertex buffers, second entry is index buffers
    static const int vb = 0;
    static const int ib = 1;
    StaticArray<buffer,2> buffers;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlPipeline
    @ingroup _priv
    @brief Metal implementation of class pipeline
*/
class mtlPipeline : public pipelineBase {
public:
    /// constructor
    mtlPipeline();
    /// destructor
    ~mtlPipeline();

    /// clear the object (called from pipelineFactory::DestroyResource())
    void Clear();

    /// render-pipeline state
    ORYOL_OBJC_TYPED_ID(MTLRenderPipelineState) mtlRenderPipelineState;
    /// depth-stencil state
    ORYOL_OBJC_TYPED_ID(MTLDepthStencilState) mtlDepthStencilState;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlShader
    @ingroup _priv
    @brief Metal implementation of class shader
*/
class mtlShader : public shaderBase {
public:
    /// constructor
    mtlShader();
    /// destructor
    ~mtlShader();

    /// clear the object
    void Clear();

    ORYOL_OBJC_TYPED_ID(MTLLibrary) mtlVertexShaderLibrary;
    ORYOL_OBJC_TYPED_ID(MTLLibrary) mtlFragmentShaderLibrary;
    ORYOL_OBJC_TYPED_ID(MTLFunction) mtlVertexShader;
    ORYOL_OBJC_TYPED_ID(MTLFunction) mtlFragmentShader;

    /// bind slots for vertex attributes (InvalidIndex if not used)
    StaticArray<int, VertexAttr::NumVertexAttrs> vsAttrIndices;

};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTexture
    @ingroup _priv
    @brief Metal implementation of class texture
*/
class mtlTexture : public textureBase {
public:
    /// constructor
    mtlTexture();
    /// destructor
    ~mtlTexture();

    /// clear the object
    void Clear();

    static const int NumSlots = GfxConfig::MaxInflightFrames;
    int updateFrameIndex;
    uint8_t numSlots;
    uint8_t activeSlot;
    StaticArray<ORYOL_OBJC_TYPED_ID(MTLTexture), NumSlots> mtlTextures;

    ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
    ORYOL_OBJC_TYPED_ID(MTLTexture) mtlDepthTex;
    ORYOL_OBJC_TYPED_ID(MTLTexture) mtlMSAATex;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlRenderPass
    @ingroup _priv
    @brief Metal implementation of renderPass
*/
class mtlRenderPass : public renderPassBase {
    // empty
};

} // namespace _priv
} // namespace Oryol


