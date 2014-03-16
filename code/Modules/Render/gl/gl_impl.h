#pragma once
//------------------------------------------------------------------------------
/**
    Central GL header file, this is BIG, make sure to only include
    this in implementation files, not header files.
*/
#if (ORYOL_WINDOWS || ORYOL_LINUX || ORYOL_OSX)
#define GLEW_STATIC (1)
#include "glew/GL/glew.h"
#elif ORYOL_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif ORYOL_PNACL
#define GL_GLEXT_PROTOTYPES
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include <ppapi/gles2/gl2ext_ppapi.h>
#elif ORYOL_EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#elif ORYOL_ANDROID
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#error "Missing platform for GL header include!"
#endif

#if ORYOL_PNACL
#define GL_UNSIGNED_INT_24_8 GL_UNSIGNED_INT_24_8_OES
#define GL_TEXTURE_3D GL_TEXTURE_3D_OES
#define GL_DEPTH_STENCIL GL_DEPTH_STENCIL_OES
#endif

// Oryol GL error checking macro
#if ORYOL_DEBUG
#define ORYOL_GL_CHECK_ERROR() o_assert(glGetError() == GL_NO_ERROR)
#else

#endif