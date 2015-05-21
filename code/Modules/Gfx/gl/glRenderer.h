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
#include "Gfx/Attrs/DisplayAttrs.h"
#include "Gfx/gl/gl_decl.h"
#include "glm/vec4.hpp"

namespace Oryol {
namespace _priv {

class meshPool;
class displayMgr;
class texture;
class drawState;
class mesh;
class programBundle;
    
class glRenderer {
public:
    /// constructor
    glRenderer();
    /// destructor
    ~glRenderer();
    
    /// setup the renderer
    void setup(displayMgr* dispMgr, meshPool* mshPool);
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
    /// apply blend color
    void applyBlendColor(const glm::vec4& color);
    /// apply draw state
    void applyDrawState(drawState* ds);
    /// apply a texture sampler variable (special case)
    void applyTexture(int32 index, const texture* tex);
    /// apply a shader variable
    template<class T> void applyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> void applyVariableArray(int32 index, const T* values, int32 numValues);
    /// clear currently assigned render target
    void clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil);
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
    
    /// invalidate bound mesh state
    void invalidateMeshState();
    /// bind vertex buffer
    void bindVertexBuffer(GLuint vb);
    /// bind index buffer
    void bindIndexBuffer(GLuint ib);
    /// bind vertex array object
    void bindVertexArrayObject(GLuint vao);
    
    /// invalidate program state
    void invalidateProgramState();
    /// invoke glUseProgram (if changed)
    void useProgram(GLuint prog);

    /// invalidate texture state
    void invalidateTextureState();
    /// bind a texture to a sampler index
    void bindTexture(int32 samplerIndex, GLenum target, GLuint tex);
    
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
    void applyStencilState(const StencilState& state, const StencilState& curState, GLenum glFace);
    /// apply blend state to use for rendering
    void applyBlendState(const BlendState& bs);
    /// apply fixed function state
    void applyRasterizerState(const RasterizerState& rs);
    /// apply program to use for rendering
    void applyProgramBundle(programBundle* progBundle, uint32 progSelMask);
    /// apply mesh to use for rendering
    void applyMesh(const mesh* msh, const programBundle* progBundle);

    bool valid;
    displayMgr* dispMgr;
    meshPool* mshPool;
    #if ORYOL_MACOS // FIXME: should be a new 'ORYOL_GL_ISCOREPROFILE' define
    GLuint globalVAO;
    #endif

    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    static GLenum mapCullFace[Face::NumFaceCodes];

    bool rtValid;
    DisplayAttrs rtAttrs;
    
    // high-level state cache
    texture* curRenderTarget;
    drawState* curDrawState;
    mesh* curMesh;
    programBundle* curProgramBundle;
    
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
    GLuint vertexArrayObject;
    GLuint program;
    
    static const int32 MaxTextureSamplers = 16;
    GLuint samplers2D[MaxTextureSamplers];
    GLuint samplersCube[MaxTextureSamplers];
};

//------------------------------------------------------------------------------
inline const DisplayAttrs&
glRenderer::renderTargetAttrs() const {
    return this->rtAttrs;
}
    
} // namespace _priv
} // namespace Oryol