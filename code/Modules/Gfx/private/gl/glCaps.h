#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glCaps
    @ingroup _priv
    @brief GL capabilities and extension wrapper
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"
#include "Gfx/private/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class glCaps {
public:
    enum Limit {
        MaxTextureSize = 0,
        MaxCubeMapTextureSize,
        MaxViewPortWidth,
        MaxViewPortHeight,
        MaxVertexAttribs,
        MaxVertexUniformComponents,
        MaxVaryingComponents,
        MaxCombinedTextureImageUnits,
        MaxVertexTextureImageUnits,
        MaxFragmentUniformComponents,
        MaxUniformBufferBindings,
        UniformBufferOffsetAlignment,

        NumLimits,
    };

    enum Feature {
        TextureCompressionDXT,
        TextureCompressionPVRTC,
        TextureCompressionETC2,
        TextureCompressionATC,
        TextureFloat,
        TextureHalfFloat,
        InstancedArrays,
        DebugOutput,
        MSAARenderTargets,
        MapBuffer,
        PackedVertexFormat_10_2,
        MultipleRenderTarget,
        Texture3D,
        TextureArray,

        NumFeatures,
    };

    enum Severity {
        SeverityHigh,
        SeverityMedium,
        SeverityLow,
    };

    enum Flavour {
        GL_3_3_CORE,
        GLES2,
        GLES3,

        InvalidFlavour,
    };

    /// setup the info values
    static void Setup(Flavour flav);
    /// discard the info values
    static void Discard();
    /// check if object has been setup
    static bool IsValid();

    /// get an integer limit value
    static int IntLimit(Limit c);
    /// test if the GL implementation has a feature
    static bool HasFeature(Feature f);
    /// test if a texture format is supported
    static bool HasTextureFormat(PixelFormat::Code fmt);
    /// return true if GL version matches 'flavour'
    static bool IsFlavour(Flavour flav);

    /// enable debug output (GL_ARB_debug_output)
    static void EnableDebugOutput(Severity s);
    /// disable debug output
    static void DisableDebugOutput();
    /// return true if debug output is enabled
    static bool IsDebugOutputEnabled();

    /// wrapper function for glVertexAttribDivisor
    static void VertexAttribDivisor(GLuint index, GLuint divisor);
    /// wrapper function for glDrawArraysInstanced
    static void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
    /// wrapper function for glDrawElementsInstanced
    static void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount);

    /// setup the limit values
    static void setupLimits(Flavour flav);
    /// setup the feature flags
    static void setupFeatures(Flavour flav);
    /// dump general GL info (extensions, constants, ...)
    static void printInfo(Flavour flav);
    /// print a GL string, optionally replace spaces with newlines
    static void printString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine);
    /// print a 1..4 dimensional integer value
    static void printInt(GLenum glEnum, const char* name, int dim);

    static struct state_t {
        bool isValid = false;
        bool isDebugOutputEnabled = false;
        Flavour flavour = InvalidFlavour;
        int intLimits[NumLimits] = { };
        bool features[NumFeatures] = { };
    } state;
};

//------------------------------------------------------------------------------
inline int
glCaps::IntLimit(Limit l) {
    o_assert_dbg(state.isValid);
    o_assert_range_dbg(l, NumLimits);
    return state.intLimits[l];
}

//------------------------------------------------------------------------------
inline bool
glCaps::HasFeature(Feature f) {
    o_assert_dbg(state.isValid);
    o_assert_dbg(f < NumFeatures);
    return state.features[f];
}

//------------------------------------------------------------------------------
inline bool
glCaps::IsFlavour(Flavour flav) {
    o_assert_dbg(state.isValid);
    return flav == state.flavour;
}

} // namespace _priv
} // namespace Oryol
