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
#include "Render/Types/State.h"
#include "Render/gl/gl_decl.h"
#include "Core/Assert.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace Render {

class mesh;
    
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
    /// bind complete mesh object
    void BindMesh(const mesh* msh);
    
    /// invalidate program state
    void InvalidateProgramState();
    /// invoke glUseProgram (if changed)
    void UseProgram(GLuint prog);
    /// bind currently selected program in program bundle
    void BindProgram(const programBundle* progBundle);

    /// invalidate texture state
    void InvalidateTextureState();
    
private:
    /// a state value union wrapper
    struct Val {
        Val() : i(0) {};
        union {
            State::Value v;
            int32 i;
            float32 f;
            bool b;
        };
    };

    /// type signature masks
    static const uint32 U = 0; // undefined
    static const uint32 V = 1; // State::Value
    static const uint32 F = 2; // float32
    static const uint32 I = 3; // int32
    static const uint32 B = 4; // bool

    /// possible parameter type signature
    enum Signature {
        Void = 0,
        B0 = B,                             ///< 1 bool
        B0_B1_B2_B3 = B|B<<3|B<<6|B<<9,     ///< 4 bool
        V0 = V,                             ///< 1 State::Value
        V0_V1 = V|V<<3,                     ///< 2 State::Value
        V0_V1_V2 = V|V<<3|V<<6,             ///< 3 State::Value
        V0_V1_V2_V3 = V|V<<3|V<<6|V<<9,     ///< 4 State::Value
        F0 = F,                             ///< 1 float32
        F0_F1 = F|F<<3,                     ///< 2 float32
        F0_F1_F2_F3 = F|F<<3|F<<6|F<<9,     ///< 4 float32
        I0 = I,                             ///< 1 int32
        I0_I1 = I|I<<3,                     ///< 2 int32
        I0_I1_I2_I3 = I|I<<3|I<<6|I<<9,     ///< 4 int32
        V0_I0 = V|I<<3,                     ///< 1 State::Value, 1 int32
        V0_I0_I1 = V|I<<3|I<<6,             ///< 1 State::Value, 2 int32
        V0_V1_I0_I1 = V|V<<3|I<<6|I<<9      ///< 2 State::Value, 2 int32
    };
    
    /// a state vector
    struct ValBlock {
        Val val[4];
    };

    /// state update callback function
    typedef void (glStateWrapper::*Callback)(const ValBlock& input);
    
    /// a state table entry
    struct Function {
    public:
        /// constructor
        Function() : cb(0), sig(Void) {};
        
        Callback cb;
        Signature sig;
    };
    
    /// setup the state-value translation table
    void setupStateTranslationTable();
    /// setup the state-vector table
    void setupStateVector();
    /// FontFace state function
    void onFrontFace(const ValBlock& input);
    /// CullFaceEnabled state function
    void onCullFaceEnabled(const ValBlock& input);
    /// CullFace state function
    void onCullFace(const ValBlock& input);
    /// DepthOffsetEnabled state function
    void onDepthOffsetEnabled(const ValBlock& input);
    /// DepthOffset state function
    void onDepthOffset(const ValBlock& input);
    /// ScissorTestEnabled state function
    void onScissorTestEnabled(const ValBlock& input);
    /// ScissorRect state function
    void onScissorRect(const ValBlock& input);
    /// StencilTestEnabled state function
    void onStencilTestEnabled(const ValBlock& input);
    /// StencilFunc state function
    void onStencilFunc(const ValBlock& input);
    /// StencilFuncSeparate state function
    void onStencilFuncSeparate(const ValBlock& input);
    /// StencilOp state function
    void onStencilOp(const ValBlock& input);
    /// StencilOpSeparate state function
    void onStencilOpSeparate(const ValBlock& input);
    /// DepthTestEnabled state function
    void onDepthTestEnabled(const ValBlock& input);
    /// DepthFunc state function
    void onDepthFunc(const ValBlock& input);
    /// BlendEnabled state function
    void onBlendEnabled(const ValBlock& input);
    /// BlendEquation state function
    void onBlendEquation(const ValBlock& input);
    /// BlendEquationSeparate state function
    void onBlendEquationSeparate(const ValBlock& input);
    /// BlendFunc state function
    void onBlendFunc(const ValBlock& input);
    /// BlendFuncSeparate state function
    void onBlendFuncSeparate(const ValBlock& input);
    /// BlendColor state function
    void onBlendColor(const ValBlock& input);
    /// DitherEnabled state function
    void onDitherEnabled(const ValBlock& input);
    /// ColorMask state function
    void onColorMask(const ValBlock& input);
    /// DepthMask state function
    void onDepthMask(const ValBlock& input);
    /// StencilMask state function
    void onStencilMask(const ValBlock& input);
    /// StencilMaskSeparate state function
    void onStencilMaskSeparate(const ValBlock& input);
    /// ClearColor state function
    void onClearColor(const ValBlock& input);
    /// ClearDepth state function
    void onClearDepth(const ValBlock& input);
    /// ClearStencil state function
    void onClearStencil(const ValBlock& input);
    /// ViewPort state function
    void onViewPort(const ValBlock& input);
    /// DepthRange state function
    void onDepthRange(const ValBlock& input);
    
    bool isValid;

    Function funcs[State::NumStateCodes];
    GLenum glEnum[State::NumStateValues];
    
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
};

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, bool b0) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(B0 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].b = b0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, bool b0, bool b1, bool b2, bool b3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(B0_B1_B2_B3 == this->funcs[c].sig);
    ValBlock values;
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
    o_assert_dbg(V0 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].v = v0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(V0_V1 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1, State::Value v2) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(V0_V1_V2 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    values.val[2].v = v2;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1, State::Value v2, State::Value v3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(V0_V1_V2_V3 == this->funcs[c].sig);
    ValBlock values;
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
    o_assert_dbg(F0 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].f = f0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0, float32 f1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(F0_F1 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].f = f0;
    values.val[1].f = f1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, float32 f0, float32 f1, float32 f2, float32 f3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(F0_F1_F2_F3 == this->funcs[c].sig);
    ValBlock values;
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
    o_assert_dbg(I0 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].i = i0;
    (this->*funcs[c].cb)(values);
}
    
//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, int32 i0, int32 i1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(I0_I1 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].i = i0;
    values.val[1].i = i1;
    (this->*funcs[c].cb)(values);
}
    
//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, int32 i0, int32 i1, int32 i2, int32 i3) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(I0_I1_I2_I3 == this->funcs[c].sig);
    ValBlock values;
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
    o_assert_dbg(V0_I0 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].v = v0;
    values.val[1].i = i0;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, int32 i0, int32 i1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(V0_I0_I1 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].v = v0;
    values.val[1].i = i0;
    values.val[2].i = i1;
    (this->*funcs[c].cb)(values);
}

//------------------------------------------------------------------------------
inline void
glStateWrapper::ApplyState(State::Code c, State::Value v0, State::Value v1, int32 i0, int32 i1) {
    o_assert_dbg((c >= 0) && (c < State::NumStateCodes));
    o_assert_dbg(V0_V1_I0_I1 == this->funcs[c].sig);
    ValBlock values;
    values.val[0].v = v0;
    values.val[1].v = v1;
    values.val[2].i = i0;
    values.val[3].i = i1;
    (this->*funcs[c].cb)(values);
}

} // namespace Render
} // namespace Oryol
