//------------------------------------------------------------------------------
//  glCaps.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gl_impl.h"
#include "Core/Assertion.h"
#include "Core/String/StringBuilder.h"
#include "glCaps.h"

namespace Oryol {
namespace _priv {

glCaps::state_t glCaps::state;

//------------------------------------------------------------------------------
void
glCaps::Setup(Flavour flav) {
    o_assert_dbg(!state.isValid);
    state = state_t();
    state.isValid = true;
    state.flavour = flav;
    
    setupFeatures(flav);
    setupLimits(flav);
    printInfo(flav);
}

//------------------------------------------------------------------------------
void
glCaps::Discard() {
    o_assert_dbg(state.isValid);
    state.isValid = false;
}

//------------------------------------------------------------------------------
bool
glCaps::IsValid() {
    return state.isValid;
}

//------------------------------------------------------------------------------
void
glCaps::setupLimits(Flavour flav) {
    ORYOL_GL_CHECK_ERROR();
    ::glGetIntegerv(GL_MAX_TEXTURE_SIZE, &state.intLimits[MaxTextureSize]);
    ::glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &state.intLimits[MaxCubeMapTextureSize]);
    ::glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &state.intLimits[MaxViewPortWidth]);
    ::glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &state.intLimits[MaxVertexAttribs]);
    ::glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &state.intLimits[MaxCombinedTextureImageUnits]);
    ::glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &state.intLimits[MaxVertexTextureImageUnits]);
    ORYOL_GL_CHECK_ERROR();
    #if ORYOL_OPENGLES2
    if (flav == GLES2) {
        ::glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &state.intLimits[MaxVertexUniformComponents]);
        ::glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &state.intLimits[MaxFragmentUniformComponents]);
    }
    #endif
    #if !ORYOL_OPENGLES2
    if (flav != GLES2) {
        ::glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &state.intLimits[MaxVertexUniformComponents]);
        ::glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &state.intLimits[MaxFragmentUniformComponents]);
    }
    #endif
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glCaps::setupFeatures(Flavour flav) {
    ORYOL_GL_CHECK_ERROR();
    if (flav != GL_3_3_CORE) {
        // non-core GL, check extensions
        StringBuilder strBuilder((const char*)::glGetString(GL_EXTENSIONS));
        ORYOL_GL_CHECK_ERROR();
        state.features[TextureCompressionDXT] = strBuilder.Contains("_texture_compression_s3tc") ||
                                                strBuilder.Contains("_compressed_texture_s3tc") ||
                                                strBuilder.Contains("_texture_compression_dxt1");
        state.features[TextureCompressionPVRTC] = strBuilder.Contains("_texture_compression_pvrtc") ||
                                                  strBuilder.Contains("_compressed_texture_pvrtc");
        state.features[TextureCompressionATC] = strBuilder.Contains("_compressed_ATC_texture") ||
                                                strBuilder.Contains("_compressed_texture_atc");
        state.features[TextureFloat] = strBuilder.Contains("_texture_float");
        state.features[InstancedArrays] = strBuilder.Contains("_instanced_arrays");
        state.features[DebugOutput] = strBuilder.Contains("_debug_output");
        if (flav == GLES2) {
            state.features[TextureHalfFloat] = strBuilder.Contains("_texture_half_float");
        }
        else {
            state.features[TextureHalfFloat] = state.features[TextureFloat];
        }
        // FIXME: check for EXT_draw_buffers
    }
    if (flav == GL_3_3_CORE) {
        // GL 3.3 Core
        state.features[InstancedArrays] = true;
        state.features[TextureFloat] = true;
        state.features[MultipleRenderTarget] = true;
        state.features[MSAARenderTargets] = true;
        state.features[PackedVertexFormat_10_2] = true;
        state.features[MapBuffer] = true;
        state.features[TextureCompressionDXT] = true;
        state.features[Texture3D] = true;
        state.features[TextureArray] = true;
    }
    else if (flav == GLES3) {
        state.features[InstancedArrays] = true;
        state.features[TextureFloat] = true;
        state.features[MultipleRenderTarget] = true;
        state.features[MSAARenderTargets] = true;
        state.features[PackedVertexFormat_10_2] = true;
        #if !ORYOL_EMSCRIPTEN
        state.features[MapBuffer] = true;
        #endif
        state.features[TextureCompressionETC2] = true;
        state.features[Texture3D] = true;
        state.features[TextureArray] = true;
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
bool
glCaps::HasTextureFormat(PixelFormat::Code fmt) {
    if (PixelFormat::IsCompressedFormat(fmt)) {
        switch (fmt) {
            case PixelFormat::DXT1:
            case PixelFormat::DXT3:
            case PixelFormat::DXT5:
                return HasFeature(TextureCompressionDXT);
            case PixelFormat::PVRTC2_RGB:
            case PixelFormat::PVRTC4_RGB:
            case PixelFormat::PVRTC2_RGBA:
            case PixelFormat::PVRTC4_RGBA:
                return HasFeature(TextureCompressionPVRTC);
            case PixelFormat::ETC2_RGB8:
            case PixelFormat::ETC2_SRGB8:
                return HasFeature(TextureCompressionETC2);
            default:
                return false;
        }
    }
    else {
        // non compressed format, always supported
        return true;
    }
}

//------------------------------------------------------------------------------
void
glCaps::VertexAttribDivisor(GLuint index, GLuint divisor) {
    if (state.features[InstancedArrays]) {
        #if ORYOL_IOS && ORYOL_OPENGLES2
        ::glVertexAttribDivisorEXT(index, divisor);
        #elif ORYOL_RASPBERRYPI
        o_error("glCaps::VertexAttribDivisor() called!\n");
        #elif ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
        ::glVertexAttribDivisorANGLE(index, divisor);
        #else
        ::glVertexAttribDivisor(index, divisor);
        #endif
    }
}

//------------------------------------------------------------------------------
void
glCaps::DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount) {
    if (state.features[InstancedArrays]) {
        #if ORYOL_IOS && ORYOL_OPENGLES2
        ::glDrawArraysInstancedEXT(mode, first, count, primcount);
        #elif ORYOL_RASPBERRYPI
        o_error("glCaps::DrawArraysInstanced() called!\n");
        #elif ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
        ::glDrawArraysInstancedANGLE(mode, first, count, primcount);
        #else
        ::glDrawArraysInstanced(mode, first, count, primcount);
        #endif
    }
}

//------------------------------------------------------------------------------
void
glCaps::DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount) {
    if (state.features[InstancedArrays]) {
        #if ORYOL_IOS && ORYOL_OPENGLES2
        ::glDrawElementsInstancedEXT(mode, count, type, indices, primcount);
        #elif ORYOL_RASPBERRYPI
        o_error("glCaps::DrawElementsInstanced() called!\n");
        #elif ORYOL_OPENGLES2 && !ORYOL_EMSCRIPTEN
        ::glDrawElementsInstancedANGLE(mode, count, type, indices, primcount);
        #else
        ::glDrawElementsInstanced(mode, count, type, indices, primcount);
        #endif
    }
}

//------------------------------------------------------------------------------
void
glCaps::printInfo(Flavour flav) {
    ORYOL_GL_CHECK_ERROR();
    printString(GL_VERSION, "GL_VERSION", false);
    printString(GL_VENDOR, "GL_VENDOR", false);
    printString(GL_RENDERER, "GL_RENDERER", false);
    printString(GL_SHADING_LANGUAGE_VERSION, "GL_SHADING_LANGUAGE_VERSION", false);
    printInt(GL_MAX_TEXTURE_SIZE, "GL_MAX_TEXTURE_SIZE", 1);
    printInt(GL_MAX_CUBE_MAP_TEXTURE_SIZE, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", 1);
    printInt(GL_MAX_VIEWPORT_DIMS, "GL_MAX_VIEWPORT_DIMS", 2);
    printInt(GL_MAX_VERTEX_ATTRIBS, "GL_MAX_VERTEX_ATTRIBS", 1);
    printInt(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", 1);
    printInt(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", 1);
    #if ORYOL_OPENGLES2
    if (flav == GLES2) {
        printInt(GL_MAX_VERTEX_UNIFORM_VECTORS, "GL_MAX_VERTEX_UNIFORM_VECTORS", 1);
        printInt(GL_MAX_FRAGMENT_UNIFORM_VECTORS, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", 1);
    }
    #endif
    #if !ORYOL_OPENGLES2
    if (flav != GLES2) {
        printInt(GL_MAX_VERTEX_UNIFORM_COMPONENTS, "GL_MAX_VERTEX_UNIFORM_COMPONENTS", 1);
        printInt(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", 1);
    }
    if (HasFeature(MultipleRenderTarget)) {
        printInt(GL_MAX_COLOR_ATTACHMENTS, "GL_MAX_COLOR_ATTACHMENTS", 1);
    }
    if (HasFeature(Texture3D)) {
        printInt(GL_MAX_3D_TEXTURE_SIZE, "GL_MAX_3D_TEXTURE_SIZE", 1);
    }
    #endif
    if (flav != GL_3_3_CORE) {
        printString(GL_EXTENSIONS, "GL_EXTENSIONS", true);
    }
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glCaps::printString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine) {
    o_assert(name);
    const char* rawStr = (const char*) ::glGetString(glEnum);
    if (rawStr) {
        String str(rawStr);
        if (replaceSpaceWithNewLine) {
            StringBuilder strBuilder;
            strBuilder.Append(" ");
            strBuilder.Append(str);
            strBuilder.SubstituteAll(" ", "\n");
            str = strBuilder.GetString();
        }
        Log::Info("%s: %s\n", name, str.AsCStr());
    }
    else {
        o_warn("::glGetString() returned nullptr!\n");
    }
}

//------------------------------------------------------------------------------
void
glCaps::printInt(GLenum glEnum, const char* name, int dim) {
    o_assert(name);
    o_assert_range(dim, 4);
    GLint value[4];
    ::glGetIntegerv(glEnum, &(value[0]));
    if (1 == dim) Log::Info("%s: %d\n", name, value[0]);
    else if (2 == dim) Log::Info("%s: %d %d\n", name, value[0], value[1]);
    else if (3 == dim) Log::Info("%s: %d %d %d\n", name, value[0], value[1], value[2]);
    else if (4 == dim) Log::Info("%s: %d %d %d %d\n", name, value[0], value[1], value[2], value[3]);
}

//------------------------------------------------------------------------------
#if !ORYOL_EMSCRIPTEN
#if defined(GL_ARB_debug_output)
#if ORYOL_WINDOWS
void __stdcall
#else
void
#endif
debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam)
{
    const char* typeStr;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:                   typeStr = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:     typeStr = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:      typeStr = "UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:             typeStr = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:             typeStr = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_OTHER_ARB:                   typeStr = "OTHER"; break;
        default:                                        typeStr = "<unknown>"; break;
    }
    const char* sevStr;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_LOW_ARB:     sevStr = "LOW"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:  sevStr = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_HIGH_ARB:    sevStr = "HIGH"; break;
        default:                            sevStr = "<unknown>"; break;
    }
    o_warn("##### OpenGL Debug Message:\n"
        "type: %s, id: %d, severity: %s\nmsg: %s\n",
        typeStr, id, sevStr, message);
}
#endif
#endif

//------------------------------------------------------------------------------
void
glCaps::EnableDebugOutput(Severity s) {
    #if defined(GL_ARB_debug_output) && !ORYOL_EMSCRIPTEN
    if (HasFeature(DebugOutput)) {
        GLenum glSeverity = GL_DONT_CARE;
        switch (s) {
            case SeverityHigh:      glSeverity = GL_DEBUG_SEVERITY_HIGH_ARB; break;
            case SeverityMedium:    glSeverity = GL_DEBUG_SEVERITY_MEDIUM_ARB; break;
            case SeverityLow:       glSeverity = GL_DEBUG_SEVERITY_LOW_ARB; break;
        }
        ::glDebugMessageControlARB(GL_DONT_CARE,        // source
                                   GL_DONT_CARE,        // type
                                   glSeverity,            // severity
                                   0,                   // count
                                   nullptr,             // ids
                                   GL_TRUE);            // enabled
        if (!state.isDebugOutputEnabled) {
            ::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            ::glDebugMessageCallbackARB(debugOutputCallback, nullptr);
            state.isDebugOutputEnabled = true;
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
glCaps::DisableDebugOutput() {
    #if defined(GL_ARB_debug_output) && !ORYOL_EMSCRIPTEN
    if (glCaps::HasFeature(glCaps::DebugOutput)) {
        ::glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        state.isDebugOutputEnabled = false;
    }
    #endif
}

//------------------------------------------------------------------------------
bool
glCaps::IsDebugOutputEnabled() {
    return state.isDebugOutputEnabled;
}

} // namespace _priv
} // namespace Oryol

