//------------------------------------------------------------------------------
//  sokolImpl.cc
//------------------------------------------------------------------------------
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#include "Core/Log.h"
#define SOKOL_IMPL
#define SOKOL_ASSERT(c) o_assert_dbg(c)
#define SOKOL_MALLOC(s) Oryol::Memory::Alloc(s)
#define SOKOL_FREE(p) Oryol::Memory::Free(p)
#define SOKOL_LOG(m) Oryol::Log::Info("%s\n",m)
#define SOKOL_UNREACHABLE o_assert_dbg(false)
#if ORYOL_OPENGL
    #if ORYOL_RASPBERRYPI
    #define GL_GLEXT_PROTOTYPES
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #elif ORYOL_WINDOWS || ORYOL_LINUX || ORYOL_MACOS
    #include "Gfx/private/flextGL.h"
    #elif ORYOL_IOS
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
    #elif ORYOL_EMSCRIPTEN
        #if ORYOL_OPENGLES2
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include <GL/glext.h>
        #else
        #include <GLES3/gl3.h>
        #endif
    #elif ORYOL_ANDROID
    #define GL_GLEXT_PROTOTYPES
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
    #else
    #error "Missing platform for GL header include!"
    #endif
#endif
#if ORYOL_OPENGLES2
#define SOKKOL_GLES2
#elif ORYOL_OPENGL3
#define SOKOL_GLES3
#elif ORYOL_OPENGL_CORE_PROFILE
#define SOKOL_GLCORE33
#elif ORYOL_D3D11
#define SOKOL_D3D11
#elif ORYOL_METAL
#error "please include sokolImpl.mm for Metal builds"
#endif
#include "sokol_gfx.h"

