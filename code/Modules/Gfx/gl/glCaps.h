#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glCaps
    @ingroup _priv
    @brief GL capabilities and extension wrapper
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/gl/gl_decl.h"

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

        NumFeatures,
    };

    enum Severity {
        SeverityHigh,
        SeverityMedium,
        SeverityLow,
    };

    /// setup the info values
    static void Setup();
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

private:
    /// setup the limit values
    static void setupLimits();
    /// setup the feature flags
    static void setupFeatures();
    /// dump general GL info (extensions, constants, ...)
    static void printInfo();
    /// print a GL string, optionally replace spaces with newlines
    static void printString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine);
    /// print a 1..4 dimensional integer value
    static void printInt(GLenum glEnum, const char* name, int dim);

    static struct state_t {
        bool isValid = false;
        bool isDebugOutputEnabled = false;
        int intLimits[NumLimits] = { };
        bool features[NumFeatures] = { };
    } state;
};

//------------------------------------------------------------------------------
inline int
glCaps::IntLimit(Limit l) {
    o_assert_range_dbg(l, NumLimits);
    return state.intLimits[l];
}

//------------------------------------------------------------------------------
inline bool
glCaps::HasFeature(Feature f) {
    o_assert_dbg(f < NumFeatures);
    return state.features[f];
}

} // namespace _priv
} // namespace Oryol
