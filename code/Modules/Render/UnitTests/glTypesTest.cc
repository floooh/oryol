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

    // PrimitiveType
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::Points) == GL_POINTS);
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::LineStrip) == GL_LINE_STRIP);
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::LineLoop) == GL_LINE_LOOP);
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::Lines) == GL_LINES);
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::TriangleStrip) == GL_TRIANGLE_STRIP);
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::TriangleFan) == GL_TRIANGLE_FAN);
    CHECK(glTypes::AsGLPrimitiveType(PrimitiveType::Triangles) == GL_TRIANGLES);
    
    // IndexType
    CHECK(glTypes::AsGLIndexType(IndexType::Index16) == GL_UNSIGNED_SHORT);
    CHECK(glTypes::AsGLIndexType(IndexType::Index32) == GL_UNSIGNED_INT);
    
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

    // Usage
    CHECK(glTypes::AsGLUsage(Usage::Immutable) == GL_STATIC_DRAW);
    CHECK(glTypes::AsGLUsage(Usage::DynamicWrite) == GL_DYNAMIC_DRAW);
    CHECK(glTypes::AsGLUsage(Usage::DynamicStream) == GL_STREAM_DRAW);

    // TextureWrapMode
    CHECK(glTypes::AsGLTextureWrapMode(TextureWrapMode::ClampToEdge) == GL_CLAMP_TO_EDGE);
    CHECK(glTypes::AsGLTextureWrapMode(TextureWrapMode::Repeat) == GL_REPEAT);
    CHECK(glTypes::AsGLTextureWrapMode(TextureWrapMode::MirroredRepeat) == GL_MIRRORED_REPEAT);
    
    // TextureFilterMode
    CHECK(glTypes::AsGLTextureFilterMode(TextureFilterMode::Nearest) == GL_NEAREST);
    CHECK(glTypes::AsGLTextureFilterMode(TextureFilterMode::Linear) == GL_LINEAR);
    CHECK(glTypes::AsGLTextureFilterMode(TextureFilterMode::NearestMipmapNearest) == GL_NEAREST_MIPMAP_NEAREST);
    CHECK(glTypes::AsGLTextureFilterMode(TextureFilterMode::NearestMipmapLinear) == GL_NEAREST_MIPMAP_LINEAR);
    CHECK(glTypes::AsGLTextureFilterMode(TextureFilterMode::LinearMipmapNearest) == GL_LINEAR_MIPMAP_NEAREST);
    CHECK(glTypes::AsGLTextureFilterMode(TextureFilterMode::LinearMipmapLinear) == GL_LINEAR_MIPMAP_LINEAR);
    
    // TextureType
    CHECK(glTypes::AsGLTextureTarget(TextureType::Texture2D) == GL_TEXTURE_2D);
    #if ORYOL_OPENGLES2
    CHECK(glTypes::AsGLTextureTarget(TextureType::Texture3D) == GL_TEXTURE_3D_OES);
    #else
    CHECK(glTypes::AsGLTextureTarget(TextureType::Texture3D) == GL_TEXTURE_3D);
    #endif
    CHECK(glTypes::AsGLTextureTarget(TextureType::TextureCube) == GL_TEXTURE_CUBE_MAP);
}
