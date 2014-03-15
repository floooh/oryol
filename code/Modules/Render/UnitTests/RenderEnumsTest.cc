//------------------------------------------------------------------------------
//  RenderEnumsTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "Render/Types/PixelFormat.h"
#include "Render/Types/PrimitiveType.h"
#include "Render/Types/TextureType.h"
#include "Render/Types/Usage.h"
#include "Render/Types/TextureWrapMode.h"
#include "Render/Types/IndexType.h"
#include "Render/Types/TextureFilterMode.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(PixelFormatTest) {
    CHECK(PixelFormat::NumPixelFormats == 15);
    
    CHECK(PixelFormat::FromString("R8G8B8A8") == PixelFormat::R8G8B8A8);
    CHECK(PixelFormat::FromString("R8G8B8") == PixelFormat::R8G8B8);
    CHECK(PixelFormat::FromString("R5G6B5") == PixelFormat::R5G6B5);
    CHECK(PixelFormat::FromString("R5G5B5A1") == PixelFormat::R5G5B5A1);
    CHECK(PixelFormat::FromString("R4G4B4A4") == PixelFormat::R4G4B4A4);
    CHECK(PixelFormat::FromString("L8") == PixelFormat::L8);
    CHECK(PixelFormat::FromString("DXT1") == PixelFormat::DXT1);
    CHECK(PixelFormat::FromString("DXT3") == PixelFormat::DXT3);
    CHECK(PixelFormat::FromString("DXT5") == PixelFormat::DXT5);
    CHECK(PixelFormat::FromString("D16") == PixelFormat::D16);
    CHECK(PixelFormat::FromString("D32") == PixelFormat::D32);
    CHECK(PixelFormat::FromString("D24S8") == PixelFormat::D24S8);
    CHECK(PixelFormat::FromString("R32F") == PixelFormat::R32F);
    CHECK(PixelFormat::FromString("PVRTC2") == PixelFormat::PVRTC2);
    CHECK(PixelFormat::FromString("PVRTC4") == PixelFormat::PVRTC4);

    CHECK(String(PixelFormat::ToString(PixelFormat::R8G8B8A8)) == "R8G8B8A8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R8G8B8)) == "R8G8B8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R5G6B5)) == "R5G6B5");
    CHECK(String(PixelFormat::ToString(PixelFormat::R5G5B5A1)) == "R5G5B5A1");
    CHECK(String(PixelFormat::ToString(PixelFormat::R4G4B4A4)) == "R4G4B4A4");
    CHECK(String(PixelFormat::ToString(PixelFormat::L8)) == "L8");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT1)) == "DXT1");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT3)) == "DXT3");
    CHECK(String(PixelFormat::ToString(PixelFormat::DXT5)) == "DXT5");
    CHECK(String(PixelFormat::ToString(PixelFormat::D16)) == "D16");
    CHECK(String(PixelFormat::ToString(PixelFormat::D32)) == "D32");
    CHECK(String(PixelFormat::ToString(PixelFormat::D24S8)) == "D24S8");
    CHECK(String(PixelFormat::ToString(PixelFormat::R32F)) == "R32F");
    CHECK(String(PixelFormat::ToString(PixelFormat::PVRTC2)) == "PVRTC2");
    CHECK(String(PixelFormat::ToString(PixelFormat::PVRTC4)) == "PVRTC4");
}

//------------------------------------------------------------------------------
TEST(TextureTypeTest) {
    CHECK(TextureType::NumTextureTypes == 3);
    CHECK(TextureType::FromString("Texture2D") == TextureType::Texture2D);
    CHECK(TextureType::FromString("Texture3D") == TextureType::Texture3D);
    CHECK(TextureType::FromString("TextureCube") == TextureType::TextureCube);
    CHECK(TextureType::FromString("Bla") == TextureType::InvalidTextureType);
    CHECK(String(TextureType::ToString(TextureType::Texture2D)) == "Texture2D");
    CHECK(String(TextureType::ToString(TextureType::Texture3D)) == "Texture3D");
    CHECK(String(TextureType::ToString(TextureType::TextureCube)) == "TextureCube");
}

//------------------------------------------------------------------------------
TEST(PrimitiveTypeTest) {
    CHECK(PrimitiveType::NumPrimitiveTypes == 7);
    
    CHECK(PrimitiveType::FromString("Points") == PrimitiveType::Points);
    CHECK(PrimitiveType::FromString("LineStrip") == PrimitiveType::LineStrip);
    CHECK(PrimitiveType::FromString("LineLoop") == PrimitiveType::LineLoop);
    CHECK(PrimitiveType::FromString("Lines") == PrimitiveType::Lines);
    CHECK(PrimitiveType::FromString("TriangleStrip") == PrimitiveType::TriangleStrip);
    CHECK(PrimitiveType::FromString("TriangleFan") == PrimitiveType::TriangleFan);
    CHECK(PrimitiveType::FromString("Triangles") == PrimitiveType::Triangles);
    
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Points)) == "Points");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::LineStrip)) == "LineStrip");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::LineLoop)) == "LineLoop");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Lines)) == "Lines");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::TriangleStrip)) == "TriangleStrip");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::TriangleFan)) == "TriangleFan");
    CHECK(String(PrimitiveType::ToString(PrimitiveType::Triangles)) == "Triangles");
}

//------------------------------------------------------------------------------
TEST(UsageTest) {
    CHECK(Usage::NumUsages == 3);
    CHECK(Usage::FromString("Immutable") == Usage::Immutable);
    CHECK(Usage::FromString("DynamicWrite") == Usage::DynamicWrite);
    CHECK(Usage::FromString("DynamicStream") == Usage::DynamicStream);
    CHECK(Usage::FromString("Bla") == Usage::InvalidUsage);
    CHECK(String(Usage::ToString(Usage::Immutable)) == "Immutable");
    CHECK(String(Usage::ToString(Usage::DynamicWrite)) == "DynamicWrite");
    CHECK(String(Usage::ToString(Usage::DynamicStream)) == "DynamicStream");
}

//------------------------------------------------------------------------------
TEST(TextureWrapMode) {
    CHECK(TextureWrapMode::NumTextureWrapModes == 3);
    CHECK(TextureWrapMode::FromString("ClampToEdge") == TextureWrapMode::ClampToEdge);
    CHECK(TextureWrapMode::FromString("Repeat") == TextureWrapMode::Repeat);
    CHECK(TextureWrapMode::FromString("MirroredRepeat") == TextureWrapMode::MirroredRepeat);
    CHECK(String(TextureWrapMode::ToString(TextureWrapMode::ClampToEdge)) == "ClampToEdge");
    CHECK(String(TextureWrapMode::ToString(TextureWrapMode::Repeat)) == "Repeat");
    CHECK(String(TextureWrapMode::ToString(TextureWrapMode::MirroredRepeat)) == "MirroredRepeat");
}

//------------------------------------------------------------------------------
TEST(IndexTypeTest) {
    CHECK(IndexType::NumIndexTypes == 3);
    CHECK(IndexType::FromString("None") == IndexType::None);
    CHECK(IndexType::FromString("Index16") == IndexType::Index16);
    CHECK(IndexType::FromString("Index32") == IndexType::Index32);
    CHECK(String(IndexType::ToString(IndexType::None)) == "None");
    CHECK(String(IndexType::ToString(IndexType::Index16)) == "Index16");
    CHECK(String(IndexType::ToString(IndexType::Index32)) == "Index32");
    CHECK(IndexType::ByteSize(IndexType::Index16) == 2);
    CHECK(IndexType::ByteSize(IndexType::Index32) == 4);
}

//------------------------------------------------------------------------------
TEST(TextureFilterModeTest) {
    CHECK(TextureFilterMode::NumTextureFilterModes == 6);
    CHECK(TextureFilterMode::FromString("Nearest") == TextureFilterMode::Nearest);
    CHECK(TextureFilterMode::FromString("Linear") == TextureFilterMode::Linear);
    CHECK(TextureFilterMode::FromString("NearestMipmapNearest") == TextureFilterMode::NearestMipmapNearest);
    CHECK(TextureFilterMode::FromString("NearestMipmapLinear") == TextureFilterMode::NearestMipmapLinear);
    CHECK(TextureFilterMode::FromString("LinearMipmapNearest") == TextureFilterMode::LinearMipmapNearest);
    CHECK(TextureFilterMode::FromString("LinearMipmapLinear") == TextureFilterMode::LinearMipmapLinear);
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::Nearest)) == "Nearest");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::Linear)) == "Linear");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::NearestMipmapNearest)) == "NearestMipmapNearest");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::NearestMipmapLinear)) == "NearestMipmapLinear");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::LinearMipmapNearest)) == "LinearMipmapNearest");
    CHECK(String(TextureFilterMode::ToString(TextureFilterMode::LinearMipmapLinear)) == "LinearMipmapLinear");
}

