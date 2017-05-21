#pragma once
//------------------------------------------------------------------------------
/**
    Central GL header file, this is BIG, make sure to only include
    this in implementation files, not header files.
*/
#if ORYOL_RASPBERRYPI
#define GL_GLEXT_PROTOTYPES
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#elif ORYOL_WINDOWS || ORYOL_LINUX || ORYOL_MACOS
#include "Gfx/private/flextgl/flextGL.h"
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

#ifndef GL_UNSIGNED_INT_24_8
    #define GL_UNSIGNED_INT_24_8 GL_UNSIGNED_INT_24_8_OES
#endif
#ifndef GL_TEXTURE_3D
    #define GL_TEXTURE_3D GL_TEXTURE_3D_OES
#endif
#ifndef GL_DEPTH_STENCIL
    #define GL_DEPTH_STENCIL GL_DEPTH_STENCIL_OES
#endif
#ifndef GL_DEPTH24_STENCIL8
    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#endif
#ifndef GL_HALF_FLOAT
    #define GL_HALF_FLOAT GL_HALF_FLOAT_OES
#endif
#ifndef GL_INT_2_10_10_10_REV
    #define GL_INT_2_10_10_10_REV 0x8D9F
#endif
#ifndef GL_UNSIGNED_INT_2_10_10_10_REV
    #define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#endif
#ifndef GL_RED
    #define GL_RED 0x1903
#endif
#ifndef GL_LUMINANCE
    #define GL_LUMINANCE 0x1909
#endif

// Oryol GL error checking macro
#if ORYOL_DEBUG
#define ORYOL_GL_CHECK_ERROR() o_assert(glGetError() == GL_NO_ERROR)
#else
#define ORYOL_GL_CHECK_ERROR() ((void)0)
#endif
