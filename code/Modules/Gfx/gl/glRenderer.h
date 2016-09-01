#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::glRenderer
    @brief OpenGL wrapper and state cache
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Attrs/DisplayAttrs.h"
#include "Gfx/Attrs/ImageDataAttrs.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/gl/glVertexAttr.h"
#include "glm/vec4.hpp"
#include "Gfx/gl/glCmdBuffer.h"

namespace Oryol {
namespace _priv {

class texture;
class pipeline;
class mesh;
class shader;
class textureBlock;

class glRenderer {
public:
    /// constructor
    glRenderer();
    /// destructor
    ~glRenderer();
    
    /// setup the renderer
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the renderer
    void discard();
    /// return true if renderer has been setup
    bool isValid() const;
    
    /// test if a feature is supported
    bool queryFeature(GfxFeature::Code feat) const;
    /// commit current frame
    void commitFrame();
    /// get the current render target attributes
    const DisplayAttrs& renderTargetAttrs() const;

    /// apply a render target (default or offscreen)
    void applyRenderTarget(texture* rt, const ClearState& clearState, bool record=true);
    /// apply viewport
    void applyViewPort(int x, int y, int width, int height, bool originTopLeft, bool record=true);
    /// apply scissor rect
    void applyScissorRect(int x, int y, int width, int height, bool originTopLeft, bool record=true);
    /// apply draw state
    void applyDrawState(pipeline* pip, mesh** meshes, int numMeshes, bool record=true);
    /// apply a shader uniform block (called after applyDrawState)
    void applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize, bool record=true);
    /// internal version of applyUniformBlock with offset into global uniform buffer
    void applyUniformBlockOffset(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, int startOffset, int byteSize);
    /// apply a group of textures
    void applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures, bool record=true);

    /// submit a draw call with primitive group index in current mesh
    void draw(int primGroupIndex, bool record=true);
    /// submit a draw call with direct primitive group
    void draw(const PrimitiveGroup& primGroup, bool record=true);
    /// submit a draw call for instanced rendering with primitive group index in current mesh
    void drawInstanced(int primGroupIndex, int numInstances, bool record=true);
    /// submit a draw call for instanced rendering with direct primitive group
    void drawInstanced(const PrimitiveGroup& primGroup, int numInstances, bool record=true);

    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int numBytes);
    /// update texture pixel data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes);
    
    /// invalidate bound mesh state
    void invalidateMeshState();
    /// bind vertex buffer with state caching
    void bindVertexBuffer(GLuint vb);
    /// bind index buffer with state caching
    void bindIndexBuffer(GLuint ib);

    /// invalidate shader state
    void invalidateShaderState();
    /// invoke glUseProgram (if changed)
    void useProgram(GLuint prog);

    /// invalidate texture state
    void invalidateTextureState();
    /// bind a texture to a sampler index
    void bindTexture(int samplerIndex, GLenum target, GLuint tex);

    /// update data in per-frame uniform-buffer (called from glCmdBuffer)
    void updateUniforms(const uint8_t* basePtr, int startOffset, int size);
    
private:
    /// setup the initial depth-stencil-state
    void setupDepthStencilState();
    /// setup the initial blend-state
    void setupBlendState();
    /// setup rasterizer state
    void setupRasterizerState();
    /// setup the big per-frame uniform buffers
    void setupUniformBuffers(const GfxSetup& gfxSetup);
    /// discard the per-frame uniform buffers
    void discardUniformBuffers();
    /// apply front/back side stencil state
    void applyStencilState(const DepthStencilState& state, const DepthStencilState& curState, GLenum glFace);

    bool valid;
    bool useCmdBuffer;
    bool useUniformBuffer;
    gfxPointers pointers;
    #if !ORYOL_OPENGLES2
    GLuint globalVAO;
    #endif
    uint64_t frameIndex;

    glCmdBuffer cmdBuffer;

    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    static GLenum mapCullFace[Face::NumFaceCodes];

    bool rtValid;
    DisplayAttrs rtAttrs;
    GfxSetup gfxSetup;
    
    // high-level state cache
    texture* curRenderTarget;
    pipeline* curPipeline;
    mesh* curPrimaryMesh;

    // GL state cache
    BlendState blendState;
    DepthStencilState depthStencilState;
    RasterizerState rasterizerState;

    GLint scissorX;
    GLint scissorY;
    GLsizei scissorWidth;
    GLsizei scissorHeight;
    
    glm::vec4 blendColor;
    
    GLint viewPortX;
    GLint viewPortY;
    GLsizei viewPortWidth;
    GLsizei viewPortHeight;
    
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint program;
    
    static const int MaxTextureSamplers = 16;
    StaticArray<GLuint, MaxTextureSamplers> samplers2D;
    StaticArray<GLuint, MaxTextureSamplers> samplersCube;
    StaticArray<glVertexAttr, VertexAttr::NumVertexAttrs> glAttrs;
    StaticArray<GLuint, VertexAttr::NumVertexAttrs> glAttrVBs;
    StaticArray<GLuint, GfxConfig::MaxInflightFrames> uniformBuffers;
    GLuint curUniformBuffer;
};

//------------------------------------------------------------------------------
inline const DisplayAttrs&
glRenderer::renderTargetAttrs() const {
    return this->rtAttrs;
}
    
} // namespace _priv
} // namespace Oryol
