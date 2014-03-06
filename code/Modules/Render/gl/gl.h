#pragma once
//------------------------------------------------------------------------------
/**
    Central GL header file.
*/
#if ORYOL_WINDOWS
#include <gl/gl.h>
#define GLEW_STATIC (1)
#include "glew/GL/glew.h"
#include "glew/GL/wglew.h"
#elif ORYOL_MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#elif ORYOL_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif ORYOL_NACL
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
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
