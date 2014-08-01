#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glStateWrapper
    @brief translate State changes into GL state changes
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"
#include "Render/Core/BlendState.h"
#include "Render/Core/DepthStencilState.h"
#include "Render/Core/RasterizerState.h"
#include "Render/gl/gl_decl.h"
#include "Core/Assert.h"
#include "glm/vec4.hpp"

namespace Oryol {
namespace Render {

class mesh;
class programBundle;
class drawState;
    
class glStateWrapper {
public:
    /// constructor
    glStateWrapper();
    /// destructor
    ~glStateWrapper();
    
    /// setup the state wrapper, sets the initial state
    void Setup();
    /// discard the state wrapper
    void Discard();
    /// return true if the state wrapper has been setup
    bool IsValid() const;
    
    /// apply draw state
    void ApplyDrawState(const drawState* ds);
    /// apply state after setting a new render target
    void ApplyRenderTargetState(int32 rtWidth, int32 rtHeight);
    /// apply view port rectangle
    void ApplyViewPort(int32 x, int32 y, int32 width, int32 height);
    /// apply scissor rectangle
    void ApplyScissorRect(int32 x, int32 y, int32 width, int32 height);
    /// apply blend color
    void ApplyBlendColor(float32 r, float32 g, float32 b, float32 a);

    /// invalidate bound mesh state
    void InvalidateMeshState();
    /// bind vertex buffer
    void BindVertexBuffer(GLuint vb);
    /// bind index buffer
    void BindIndexBuffer(GLuint ib);
    /// bind vertex array object
    void BindVertexArrayObject(GLuint vao);
    
    /// invalidate program state
    void InvalidateProgramState();
    /// invoke glUseProgram (if changed)
    void UseProgram(GLuint prog);

    /// invalidate texture state
    void InvalidateTextureState();
    /// bind a texture to a sampler index
    void BindTexture(int32 samplerIndex, GLenum target, GLuint tex);
    
    /// clear current render target
    void Clear(Channel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil);
    
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
    void applyProgram(programBundle* progBundle, uint32 progSelMask);
    /// apply mesh to use for rendering
    void applyMesh(const mesh* msh, const programBundle* progBundle);

    bool isValid;
    
    // global VAO for core profile platforms
    #if ORYOL_MACOS
    GLuint globalVAO;
    #endif

    BlendState curBlendState;
    DepthStencilState curDepthStencilState;
    RasterizerState curRasterizerState;
    
    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    static GLenum mapCullFace[Face::NumFaceCodes];
    
    GLint curScissorX;
    GLint curScissorY;
    GLsizei curScissorWidth;
    GLsizei curScissorHeight;
    
    GLclampf curBlendColorR;
    GLclampf curBlendColorG;
    GLclampf curBlendColorB;
    GLclampf curBlendColorA;
    
    GLint curViewPortX;
    GLint curViewPortY;
    GLsizei curViewPortWidth;
    GLsizei curViewPortHeight;
    
    GLuint curVertexBuffer;
    GLuint curIndexBuffer;
    GLuint curVertexArrayObject;
    GLuint curProgram;
    
    static const int32 MaxTextureSamplers = 16;
    GLuint samplers2D[MaxTextureSamplers];
    GLuint samplersCube[MaxTextureSamplers];
};

} // namespace Render
} // namespace Oryol
