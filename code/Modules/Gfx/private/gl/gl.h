#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/private/gl.h
    @brief include the right GL headers for the target platform
*/
#if !ORYOL_OPENGL
#error "attempting to include GL headers on non-GL platform"
#endif
#if ORYOL_RASPBERRYPI
#define GL_GLEXT_PROTOTYPES
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#elif ORYOL_WINDOWS || ORYOL_LINUX || ORYOL_MACOS
#include "Gfx/private/gl/flextGL.h"
#elif ORYOL_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif ORYOL_EMSCRIPTEN
    #if ORYOL_OPENGLES2
    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
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
