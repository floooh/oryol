//------------------------------------------------------------------------------
//  PixelFormatTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/PixelFormat.h"
#include "Core/String/String.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

TEST(PixelFormatTest) {
    CHECK(PixelFormat::NumPixelFormats == 17);
    
    CHECK(PixelFormat::FromString("X8R8G8B8") == PixelFormat::X8R8G8B8);
    CHECK(PixelFormat::FromString("A8R8G8B8") == PixelFormat::A8R8G8B8);
    CHECK(PixelFormat::FromString("R8G8B8") == PixelFormat::R8G8B8);
    CHECK(PixelFormat::FromString("R5G6B5") == PixelFormat::R5G6B5);
    CHECK(PixelFormat::FromString("A1R5G5B5") == PixelFormat::A1R5G5B5);
    CHECK(PixelFormat::FromString("A4R4G4B4") == PixelFormat::A4R4G4B4);
    CHECK(PixelFormat::FromString("L8") == PixelFormat::L8);
    CHECK(PixelFormat::FromString("DXT1") == PixelFormat::DXT1);
    CHECK(PixelFormat::FromString("DXT3") == PixelFormat::DXT3);
    CHECK(PixelFormat::FromString("DXT5") == PixelFormat::DXT5);
    CHECK(PixelFormat::FromString("D16") == PixelFormat::D16);
    CHECK(PixelFormat::FromString("D24") == PixelFormat::D24);
    CHECK(PixelFormat::FromString("D32") == PixelFormat::D32);
    CHECK(PixelFormat::FromString("D24S8") == PixelFormat::D24S8);
    CHECK(PixelFormat::FromString("R32F") == PixelFormat::R32F);
    CHECK(PixelFormat::FromString("PVRTC2") == PixelFormat::PVRTC2);
    CHECK(PixelFormat::FromString("PVRTC4") == PixelFormat::PVRTC4);

    CHECK(String(PixelFormat::ToString(PixelFormat::X8R8G8B8)) == "X8R8G8B8");
    CHECK(String(PixelFormat::ToString(PixelFormat::A8R8G8B8)) == "A8R8G8B8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R8G8B8)) == "R8G8B8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R5G6B5)) == "R5G6B5");
    CHECK(String(PixelFormat::ToString(PixelFormat::A1R5G5B5)) == "A1R5G5B5");
    CHECK(String(PixelFormat::ToString(PixelFormat::A4R4G4B4)) == "A4R4G4B4");
    CHECK(String(PixelFormat::ToString(PixelFormat::L8)) == "L8");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT1)) == "DXT1");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT3)) == "DXT3");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT5)) == "DXT5");
    CHECK(String(PixelFormat::ToString(PixelFormat::D16)) == "D16");
    CHECK(String(PixelFormat::ToString(PixelFormat::D24)) == "D24");
    CHECK(String(PixelFormat::ToString(PixelFormat::D32)) == "D32");
    CHECK(String(PixelFormat::ToString(PixelFormat::D24S8)) == "D24S8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R32F)) == "R32F");
    CHECK(String(PixelFormat::ToString(PixelFormat::PVRTC2)) == "PVRTC2");
    CHECK(String(PixelFormat::ToString(PixelFormat::PVRTC4)) == "PVRTC4");
}
