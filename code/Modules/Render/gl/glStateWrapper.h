#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glStateWrapper
    @brief translate State changes into GL state changes
    
    The glStateWrapper offers a simplified interface to the GL state,
    and filters redundant state changes. There's only a limited combination
    of state parameter types in GL, which is handled by a handful of 
    ApplyState method overrides.
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"
#include "Render/Core/BlendState.h"
#include "Render/Core/DepthStencilState.h"
#include "Render/Core/RasterizerState.h"
#include "Render/gl/gl_decl.h"
#include "Core/Assert.h"

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

    /// apply state
    void ApplyState(State::Code state, float32 f0);
    /// apply state
    void ApplyState(State::Code state, float32 f0, float32 f1);
    /// apply state
    void ApplyState(State::Code state, float32 f0, float32 f1, float32 f2, float32 f3);
    /// apply state
    void ApplyState(State::Code state, int32 i0);
    /// apply state
    void ApplyState(State::Code state, int32 i0, int32 i1, int32 i2, int32 i3);
    
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
    
private:
    
    /// state update callback function
    typedef void (glStateWrapper::*Callback)(const State::Vector& input);
    
    /// a state table entry
    struct Function {
    public:
        /// constructor
        Function() : cb(0), sig(State::Void) {};
        
        Callback cb;
        State::Signature sig;
    };
    
    /// setup the jump table
    void setupJumpTable();
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

    /// DepthOffset state function
    void onDepthOffset(const State::Vector& input);
    /// ScissorRect state function
    void onScissorRect(const State::Vector& input);
    /// BlendColor state function
    void onBlendColor(const State::Vector& input);
    /// ClearColor state function
    void onClearColor(const State::Vector& input);
    /// ClearDepth state function
    void onClearDepth(const State::Vector& input);
    /// ClearStencil state function
    void onClearStencil(const State::Vector& input);
    /// ViewPort state function
    void onViewPort(const State::Vector& input);
    
    bool isValid;
    
    // global VAO for core profile platforms
    #if ORYOL_MACOS
    GLuint globalVAO;
    #endif

    Function funcs[State::NumStateCodes];
    
    BlendState curBlendState;
    DepthStencilState curDepthStencilState;
    RasterizerState curRasterizerState;
    
    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    static GLenum mapCullFace[Face::NumFaceCodes];
    
    GLfloat curDepthOffsetFactor;
    GLfloat curDepthOffsetUnits;
    
    GLint curScissorLeft;
    GLint curScissorBottom;
    GLsizei curScissorWidth;
    GLsizei curScissorHeight;
    
    GLclampf curBlendColorR;
    GLclampf curBlendColorG;
    GLclampf curBlendColorB;
    GLclampf curBlendColorA;
    
    GLclampf curClearColorR;
    GLclampf curClearColorG;
    GLclampf curClearColorB;
    GLclampf curClearColorA;
    GLclampf curClearDepth;
    GLint curClearStencil;
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

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::F0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].f = f0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0, float32 f1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::F0_F1 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].f = f0;
    values.val[1].f = f1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0, float32 f1, float32 f2, float32 f3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::F0_F1_F2_F3 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].f = f0;
    values.val[1].f = f1;
    values.val[2].f = f2;
    values.val[3].f = f3;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, int32 i0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::I0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].i = i0;
    (this->*funcs[c].cb)(values);
}
    
//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, int32 i0, int32 i1, int32 i2, int32 i3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::I0_I1_I2_I3 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].i = i0;
    values.val[1].i = i1;
    values.val[2].i = i2;
    values.val[3].i = i3;
    (this->*funcs[c].cb)(values);
}

} // namespace Render
} // namespace Oryol
