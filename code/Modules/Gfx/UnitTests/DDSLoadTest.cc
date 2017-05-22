//------------------------------------------------------------------------------
//  DDSLoadTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "HttpFS/HTTPFileSystem.h"
#include "IO/IO.h"
#define GLIML_ASSERT o_assert
#include "gliml.h"
#include <thread>
#include <chrono>

using namespace Oryol;

TEST(DDSLoadTest) {
#if !ORYOL_EMSCRIPTEN && !ORYOL_UNITTESTS_HEADLESS
    Core::Setup();

    // setup an IO facade, and associate http: with the HTTPFileSystem
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    IO::Setup(ioSetup);

    // DXT1
    Ptr<IORead> req = IO::LoadFile("http://floooh.github.com/oryol/data/lok_dxt1.dds");
    while (!req->Handled) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    CHECK(req->Status == IOStatus::OK);
    if (req->Status == IOStatus::OK) {
        
        // parse DDS data with gliml
        const int size = req->Data.Size();
        const void* data = req->Data.Data();
        CHECK(gliml::is_dds(data, size));
        gliml::context ctx;
        ctx.enable_dxt(true);
        CHECK(ctx.load_dds(data, size));
        CHECK(ctx.error() == GLIML_SUCCESS);
        CHECK(ctx.texture_target() == GLIML_GL_TEXTURE_2D);
        CHECK(ctx.is_compressed());
        CHECK(ctx.is_2d());
        CHECK(!ctx.is_3d());
        CHECK(ctx.num_faces() == 1);
        CHECK(ctx.num_mipmaps(0) == 9);
        CHECK(ctx.image_target(0) == GLIML_GL_TEXTURE_2D);
        CHECK(ctx.image_internal_format() == GLIML_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
        CHECK(ctx.image_format() == GLIML_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
        CHECK(ctx.image_type() == 0);
        int w = 256, h = 256;
        for (int i = 0; i < 9; i++) {
            CHECK(ctx.image_width(0, i) == (w >> i));
            CHECK(ctx.image_height(0, i) == (h >> i));
            CHECK(ctx.image_depth(0, i) == 1);
            int mipSize = (w>>(2+i)) * (h>>(2+i)) * 8;
            if (mipSize < 8) mipSize = 8;
            CHECK(ctx.image_size(0, i) == mipSize);
        }
    }
    
    // DXT3
    req = IO::LoadFile("http://floooh.github.com/oryol/data/lok_dxt3.dds");
    while (!req->Handled) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    CHECK(req->Status == IOStatus::OK);
    if (req->Status == IOStatus::OK) {
        
        // parse DDS data with gliml
        const int size = req->Data.Size();
        const void* data = req->Data.Data();
        CHECK(gliml::is_dds(data, size));
        gliml::context ctx;
        ctx.enable_dxt(true);
        CHECK(ctx.load_dds(data, size));
        CHECK(ctx.error() == GLIML_SUCCESS);
        CHECK(ctx.texture_target() == GLIML_GL_TEXTURE_2D);
        CHECK(ctx.is_compressed());
        CHECK(ctx.is_2d());
        CHECK(!ctx.is_3d());
        CHECK(ctx.num_faces() == 1);
        CHECK(ctx.num_mipmaps(0) == 9);
        CHECK(ctx.image_target(0) == GLIML_GL_TEXTURE_2D);
        CHECK(ctx.image_internal_format() == GLIML_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
        CHECK(ctx.image_format() == GLIML_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
        CHECK(ctx.image_type() == 0);
        int w = 256, h = 256;
        for (int i = 0; i < 9; i++) {
            CHECK(ctx.image_width(0, i) == (w >> i));
            CHECK(ctx.image_height(0, i) == (h >> i));
            CHECK(ctx.image_depth(0, i) == 1);
            int mipSize = (w>>(2+i)) * (h>>(2+i)) * 16;
            if (mipSize < 8) mipSize = 16;
            CHECK(ctx.image_size(0, i) == mipSize);
        }
    }

    // DXT5
    req = IO::LoadFile("http://floooh.github.com/oryol/data/lok_dxt5.dds");
    while (!req->Handled) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    CHECK(req->Status == IOStatus::OK);
    if (req->Status == IOStatus::OK) {
        
        // parse DDS data with gliml
        const int size = req->Data.Size();
        const void* data = req->Data.Data();
        CHECK(gliml::is_dds(data, size));
        gliml::context ctx;
        ctx.enable_dxt(true);
        CHECK(ctx.load_dds(data, size));
        CHECK(ctx.error() == GLIML_SUCCESS);
        CHECK(ctx.texture_target() == GLIML_GL_TEXTURE_2D);
        CHECK(ctx.is_compressed());
        CHECK(ctx.is_2d());
        CHECK(!ctx.is_3d());
        CHECK(ctx.num_faces() == 1);
        CHECK(ctx.num_mipmaps(0) == 9);
        CHECK(ctx.image_target(0) == GLIML_GL_TEXTURE_2D);
        CHECK(ctx.image_internal_format() == GLIML_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
        CHECK(ctx.image_format() == GLIML_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
        CHECK(ctx.image_type() == 0);
        int w = 256, h = 256;
        for (int i = 0; i < 9; i++) {
            CHECK(ctx.image_width(0, i) == (w >> i));
            CHECK(ctx.image_height(0, i) == (h >> i));
            CHECK(ctx.image_depth(0, i) == 1);
            int mipSize = (w>>(2+i)) * (h>>(2+i)) * 16;
            if (mipSize < 8) mipSize = 16;
            CHECK(ctx.image_size(0, i) == mipSize);
        }
    }
    
    // FIXME: RGBA, LUM

    req.invalidate();
    IO::Discard();
    Core::Discard();
#endif
}
