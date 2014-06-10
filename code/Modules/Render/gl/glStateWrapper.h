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
#include "Render/Core/DepthStencilState.h"
#include "Render/gl/gl_decl.h"
#include "Core/Assert.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace Render {

class mesh;
class stateBlock;
    
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
    
    /// apply depth-stencil state
    void ApplyDepthStencilState(depthStencilState* dss);
    /// apply state block
    void ApplyStateBlock(stateBlock* sb);
    /// apply state
    void ApplyState(State::Code state, bool b0);
    /// apply state
    void ApplyState(State::Code state, bool b0, bool b1, bool b2, bool b3);
    /// apply state
    void ApplyState(State::Code state, State::Value v0);
    /// apply state
    void ApplyState(State::Code state, State::Value v0, State::Value v1);
    /// apply state
    void ApplyState(State::Code state, State::Value v0, State::Value v1, State::Value v2);
    /// apply state
    void ApplyState(State::Code state, State::Value v0, State::Value v1, State::Value v2, State::Value v3);
    /// apply state
    void ApplyState(State::Code state, float32 f0);
    /// apply state
    void ApplyState(State::Code state, float32 f0, float32 f1);
    /// apply state
    void ApplyState(State::Code state, float32 f0, float32 f1, float32 f2, float32 f3);
    /// apply state
    void ApplyState(State::Code state, int32 i0);
    /// apply state
    void ApplyState(State::Code state, int32 i0, int32 i1);
    /// apply state
    void ApplyState(State::Code state, int32 i0, int32 i1, int32 i2, int32 i3);
    /// apply state
    void ApplyState(State::Code state, State::Value v0, int32 i0);
    /// apply state
    void ApplyState(State::Code state, State::Value v0, int32 i0, int32 i1);
    /// apply state
    void ApplyState(State::Code state, State::Value v0, State::Value v1, int32 i0, int32 i1);
    
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
    /// bind currently selected program in program bundle
    void BindProgram(const programBundle* progBundle);

    /// invalidate texture state
    void InvalidateTextureState();
    /// bind a texture to a sampler index
    void BindTexture(int32 samplerIndex, GLenum target, GLuint tex);

    /// bind mesh, program may be needed for vertex attrib binding
    void BindMesh(const mesh* msh, const programBundle* progBundle);
    
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
    /// FontFace state function
    void onFrontFace(const State::Vector& input);
    /// CullFaceEnabled state function
    void onCullFaceEnabled(const State::Vector& input);
    /// CullFace state function
    void onCullFace(const State::Vector& input);
    /// DepthOffsetEnabled state function
    void onDepthOffsetEnabled(const State::Vector& input);
    /// DepthOffset state function
    void onDepthOffset(const State::Vector& input);
    /// ScissorTestEnabled state function
    void onScissorTestEnabled(const State::Vector& input);
    /// ScissorRect state function
    void onScissorRect(const State::Vector& input);
    /// StencilTestEnabled state function
    void onStencilTestEnabled(const State::Vector& input);
    /// StencilFunc state function
    void onStencilFunc(const State::Vector& input);
    /// StencilFuncSeparate state function
    void onStencilFuncSeparate(const State::Vector& input);
    /// StencilOp state function
    void onStencilOp(const State::Vector& input);
    /// StencilOpSeparate state function
    void onStencilOpSeparate(const State::Vector& input);
    /// DepthTestEnabled state function
    void onDepthTestEnabled(const State::Vector& input);
    /// DepthFunc state function
    void onDepthFunc(const State::Vector& input);
    /// BlendEnabled state function
    void onBlendEnabled(const State::Vector& input);
    /// BlendEquation state function
    void onBlendEquation(const State::Vector& input);
    /// BlendEquationSeparate state function
    void onBlendEquationSeparate(const State::Vector& input);
    /// BlendFunc state function
    void onBlendFunc(const State::Vector& input);
    /// BlendFuncSeparate state function
    void onBlendFuncSeparate(const State::Vector& input);
    /// BlendColor state function
    void onBlendColor(const State::Vector& input);
    /// DitherEnabled state function
    void onDitherEnabled(const State::Vector& input);
    /// ColorMask state function
    void onColorMask(const State::Vector& input);
    /// DepthMask state function
    void onDepthMask(const State::Vector& input);
    /// StencilMask state function
    void onStencilMask(const State::Vector& input);
    /// StencilMaskSeparate state function
    void onStencilMaskSeparate(const State::Vector& input);
    /// ClearColor state function
    void onClearColor(const State::Vector& input);
    /// ClearDepth state function
    void onClearDepth(const State::Vector& input);
    /// ClearStencil state function
    void onClearStencil(const State::Vector& input);
    /// ViewPort state function
    void onViewPort(const State::Vector& input);
    /// DepthRange state function
    void onDepthRange(const State::Vector& input);
    
    bool isValid;

    Function funcs[State::NumStateCodes];
    
    struct {
        CompareFunc::Code depthCompareFunc;
        bool depthWriteEnabled;
        bool stencilTestEnabled;
        struct {
            StencilOp::Code stencilFailOp;
            StencilOp::Code depthFailOp;
            StencilOp::Code depthStencilPassOp;
            CompareFunc::Code stencilCompareFunc;
            uint32 stencilReadMask;
            uint32 stencilWriteMask;
        } stencilState[Face::NumSides];
    } curDepthStencilState;
    
    GLenum curFrontFaceMode;
    bool curCullFaceEnabled;
    GLenum curCullFaceMode;
    
    bool curDepthOffsetEnabled;
    GLfloat curDepthOffsetFactor;
    GLfloat curDepthOffsetUnits;
    
    bool curScissorTestEnabled;
    GLint curScissorLeft;
    GLint curScissorBottom;
    GLsizei curScissorWidth;
    GLsizei curScissorHeight;
    
    bool curStencilTestEnabled;
    GLenum curStencilFunc[2];
    GLint curStencilFuncRef[2];
    GLuint curStencilFuncMask[2];
    GLenum curStencilOpSFail[2];
    GLenum curStencilOpDpFail[2];
    GLenum curStencilOpDpPass[2];
    
    bool curDepthTestEnabled;
    GLenum curDepthFunc;
    
    bool curBlendEnabled;
    GLenum curBlendEquationRGB;
    GLenum curBlendEquationAlpha;
    GLenum curBlendFuncSrcRGB;
    GLenum curBlendFuncSrcAlpha;
    GLenum curBlendFuncDstRGB;
    GLenum curBlendFuncDstAlpha;
    GLclampf curBlendColorR;
    GLclampf curBlendColorG;
    GLclampf curBlendColorB;
    GLclampf curBlendColorA;
    
    bool curDitherEnabled;
    bool curColorMaskR;
    bool curColorMaskG;
    bool curColorMaskB;
    bool curColorMaskA;
    bool curDepthMask;
    GLuint curStencilMask[2];
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
    GLfloat curDepthRangeNear;
    GLfloat curDepthRangeFar;
    
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
glStateWrapper::ApplyState(State::Code c, bool b0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::B0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].b = b0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, bool b0, bool b1, bool b2, bool b3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::B0_B1_B2_B3 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].b = b0;
    values.val[1].b = b1;
    values.val[2].b = b2;
    values.val[3].b = b3;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0_V1 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1, State::Value v2) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0_V1_V2 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    values.val[2].v = v2;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1, State::Value v2, State::Value v3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0_V1_V2_V3 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    values.val[2].v = v2;
    values.val[3].v = v3;
    (this->*funcs[c].cb)(values);
}

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
glStateWrapper::ApplyState(State::Code c, int32 i0, int32 i1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::I0_I1 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].i = i0;
    values.val[1].i = i1;
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

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, int32 i0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0_I0 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    values.val[1].i = i0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, int32 i0, int32 i1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0_I0_I1 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    values.val[1].i = i0;
    values.val[2].i = i1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1, int32 i0, int32 i1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(State::V0_V1_I0_I1 == this->funcs[c].sig);
    State::Vector values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    values.val[2].i = i0;
    values.val[3].i = i1;
    (this->*funcs[c].cb)(values);
}

} // namespace Render
} // namespace Oryol
