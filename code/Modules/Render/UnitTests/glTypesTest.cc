//------------------------------------------------------------------------------
//  glTypesTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/gl/gl_impl.h"
#include "Render/gl/glTypes.h"

using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(glTypesTest) {

    // glTexImage format
    // FIXME: incomplete
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R8G8B8A8) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R8G8B8) == GL_RGB);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R5G6B5) == GL_RGB);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R5G5B5A1) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::R4G4B4A4) == GL_RGBA);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D16) == GL_DEPTH_COMPONENT);
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D32) == GL_DEPTH_COMPONENT);
    #if ORYOL_OPENGLES2
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D24S8) == GL_DEPTH_STENCIL_OES);
    #else
    CHECK(glTypes::AsGLTexImageFormat(PixelFormat::D24S8) == GL_DEPTH_STENCIL);
    #endif
    
    // glTexImage type
    // FIXME: incomplete
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R8G8B8A8) == GL_UNSIGNED_BYTE);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R8G8B8) == GL_UNSIGNED_BYTE);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R5G6B5) == GL_UNSIGNED_SHORT_5_6_5);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R5G5B5A1) == GL_UNSIGNED_SHORT_5_5_5_1);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::R4G4B4A4) == GL_UNSIGNED_SHORT_4_4_4_4);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D16) == GL_UNSIGNED_SHORT);
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D32) == GL_UNSIGNED_INT);
    #if ORYOL_OPENGLES2
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D24S8) == GL_UNSIGNED_INT_24_8_OES);
    #else
    CHECK(glTypes::AsGLTexImageType(PixelFormat::D24S8) == GL_UNSIGNED_INT_24_8);
    #endif
}
