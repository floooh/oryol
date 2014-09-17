//------------------------------------------------------------------------------
//  glTypesTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glTypes.h"

using namespace Oryol;
using namespace _priv;

//------------------------------------------------------------------------------
TEST(glTypesTest) {

    // glTexImage format
    // FIXME: incomplete
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::RGBA8) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::RGB8) == GL_RGB);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R5G6B5) == GL_RGB);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R5G5B5A1) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::RGBA4) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::RGBA32F) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::RGBA16F) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D16) == GL_DEPTH_COMPONENT);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D32) == GL_DEPTH_COMPONENT);
    #if ORYOL_OPENGLES2
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D24S8) == GL_DEPTH_STENCIL_OES);
    #else
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D24S8) == GL_DEPTH_STENCIL);
    #endif
    
    // glTexImage type
    // FIXME: incomplete
    CHECK(glTypes::AsGLTexImageType(PixelFormat::RGBA8) == GL_UNSIGNED_BYTE);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::RGB8) == GL_UNSIGNED_BYTE);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R5G6B5) == GL_UNSIGNED_SHORT_5_6_5);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R5G5B5A1) == GL_UNSIGNED_SHORT_5_5_5_1);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::RGBA4) == GL_UNSIGNED_SHORT_4_4_4_4);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D16) == GL_UNSIGNED_SHORT);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D32) == GL_UNSIGNED_INT);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::RGBA32F) == GL_FLOAT);
    #if ORYOL_OPENGLES2
    CHECK(glTypes::AsGLTexImageType(PixelFormat::RGBA16F) == GL_HALF_FLOAT_OES);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D24S8) == GL_UNSIGNED_INT_24_8_OES);
    #else
    CHECK(glTypes::AsGLTexImageType(PixelFormat::RGBA16F) == GL_HALF_FLOAT);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D24S8) == GL_UNSIGNED_INT_24_8);
    #endif
}
