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
    void applyViewPort(int x, int y, int width, int height, bool originTopLeft);
    /// apply scissor rect
    void applyScissorRect(int x, int y, int width, int height, bool originTopLeft);
    /// apply draw state
    void applyDrawState(pipeline* pip, mesh** meshes, int numMeshes);
    /// apply a shader uniform block (called after applyDrawState)
    void applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, int64_t layoutHash, const uint8_t* ptr, int byteSize);
    /// apply a group of textures
    void applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures);
    /// submit a draw call with primitive group index in current mesh
    void draw(int primGroupIndex);
    /// submit a draw call with direct primitive group
    void draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering with primitive group index in current mesh
    void drawInstanced(int primGroupIndex, int numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    void drawInstanced(const PrimitiveGroup& primGroup, int numInstances);
    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int numBytes);
    /// update texture pixel data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void readPixels(void* buf, int bufNumBytes);
    
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
    
private:
    /// setup the initial depth-stencil-state
    void setupDepthStencilState();
    /// setup the initial blend-state
    void setupBlendState();
    /// setup rasterizer state
    void setupRasterizerState();
    /// apply depth-stencil state to use for rendering
    void applyDepthStencilState(const DepthStencilState& dss);
    /// apply front/back side stencil state
    void applyStencilState(const DepthStencilState& state, const DepthStencilState& curState, GLenum glFace);
    /// apply blend state to use for rendering
    void applyBlendState(const BlendState& bs);
    /// apply fixed function state
    void applyRasterizerState(const RasterizerState& rs);
    /// apply meshes
    void applyMeshes(pipeline* pip, mesh** meshes, int numMeshes);

    bool valid;
    gfxPointers pointers;
    #if !ORYOL_OPENGLES2
    GLuint globalVAO;
    #endif

    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    static GLenum mapCullFace[Face::NumFaceCodes];

    bool rtValid;
    DisplayAttrs rtAttrs;
    GfxSetup gfxSetup;
    int frameIndex;
    
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
    GLuint samplers2D[MaxTextureSamplers];
    GLuint samplersCube[MaxTextureSamplers];
    glVertexAttr glAttrs[VertexAttr::NumVertexAttrs];
    GLuint glAttrVBs[VertexAttr::NumVertexAttrs];
};

//------------------------------------------------------------------------------
inline const DisplayAttrs&
glRenderer::renderTargetAttrs() const {
    return this->rtAttrs;
}
    
} // namespace _priv
} // namespace Oryol
